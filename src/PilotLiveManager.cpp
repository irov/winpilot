#include "Pilot/PilotLiveManager.h"
#ifndef PILOT_NO_LIVEKIT
#include "PilotLiveKitPublisher.h"
#endif
#include "Pilot/PilotHttpClient.h"
#include "Pilot/PilotScreenCaptureProvider.h"
#include "Pilot/PilotException.h"
#include "Pilot/PilotLog.h"

#include <chrono>
#include <algorithm>

namespace Pilot {

// ── Clamp helper ──

static int clampInt(int value, int lo, int hi) {
    return std::min(std::max(value, lo), hi);
}

// ── LiveSettings ──

PilotLiveManager::LiveSettings PilotLiveManager::LiveSettings::low() {
    return {"low", 540, 2, 500};
}

PilotLiveManager::LiveSettings PilotLiveManager::LiveSettings::balanced() {
    return {"balanced", 720, 3, 400};
}

PilotLiveManager::LiveSettings PilotLiveManager::LiveSettings::high() {
    return {"high", 1080, 4, 300};
}

PilotLiveManager::LiveSettings PilotLiveManager::LiveSettings::fromPayload(const PilotJson& payload) {
    if (payload.isNull()) {
        return low();
    }

    PilotString preset = payload.optString("preset", "low");
    LiveSettings base;
    if (preset == "balanced") {
        base = balanced();
    } else if (preset == "high") {
        base = high();
    } else {
        base = low();
        preset = "low";
    }

    return {
        preset,
        clampInt(payload.optInt("max_dimension", base.maxDimension), 240, 1440),
        clampInt(payload.optInt("fps", base.framesPerSecond), 1, 6),
        static_cast<int64_t>(clampInt(payload.optInt("action_poll_interval_ms",
            static_cast<int>(base.actionPollIntervalMs)), 200, 2000))
    };
}

// ── PilotLiveManager ──

PilotLiveManager::PilotLiveManager(PilotHttpClient& httpClient)
    : m_httpClient(httpClient)
    , m_captureProvider(nullptr)
    , m_liveSettings(LiveSettings::low())
#ifndef PILOT_NO_LIVEKIT
    , m_publisher(new PilotLiveKitPublisher())
#endif
{
}

PilotLiveManager::~PilotLiveManager() {
    shutdown();
}

void PilotLiveManager::setScreenCaptureProvider(PilotScreenCaptureProvider* provider) {
    std::lock_guard<PilotMutex> lock(m_mutex);
    m_captureProvider = provider;
}

void PilotLiveManager::setOnLiveModeChanged(PilotFunction<void(bool, int64_t)> callback) {
    std::lock_guard<PilotMutex> lock(m_mutex);
    m_onLiveModeChanged = callback;
}

bool PilotLiveManager::isLive() const {
    return m_isLive.load();
}

PilotJson PilotLiveManager::handleStart(const PilotString& sessionToken, const PilotJson& payload) {
#ifdef PILOT_NO_LIVEKIT
    PilotLog::w("Live streaming is not supported on this platform (x86)");
    return buildAck(false, "live_not_supported_on_x86");
#else
    bool wasLive = m_isLive.load();
    stopLiveRuntime();

    if (wasLive) {
        std::lock_guard<PilotMutex> lock(m_mutex);
        if (m_onLiveModeChanged) {
            m_onLiveModeChanged(false, 0);
        }
    }

    {
        std::lock_guard<PilotMutex> lock(m_mutex);
        if (!m_captureProvider) {
            PilotLog::e("Live start failed: no screen capture provider set");
            return buildAck(false, "no_screen_capture_provider");
        }
    }

    try {
        LiveSettings requested = LiveSettings::fromPayload(payload);
        PublisherSession session = fetchPublisherSession(sessionToken, requested);
        LiveSettings settings = session.settings;

        {
            std::lock_guard<PilotMutex> lock(m_mutex);
            m_isLive.store(true);
            m_liveSettings = settings;
            m_captureProvider->onLiveSettingsChanged(settings.maxDimension, settings.framesPerSecond);
        }

        // Connect to LiveKit room and publish video track
        m_publisher->start(session.serverUrl, session.participantToken,
                           settings.maxDimension, settings.maxDimension);

        // Start frame capture thread
        m_captureRunning.store(true);
        m_captureThread = PilotThread([this] { frameCaptureLoop(); });

        {
            std::lock_guard<PilotMutex> lock(m_mutex);
            if (m_onLiveModeChanged) {
                m_onLiveModeChanged(true, settings.actionPollIntervalMs);
            }
        }

        PilotLog::i("Live started (preset=%s, %dx%d@%dfps)",
            settings.presetName.c_str(), settings.maxDimension, settings.maxDimension,
            settings.framesPerSecond);

        PilotJson ack = buildAck(true, "live_started");
        ack.put("preset", PilotJson(settings.presetName));
        ack.put("max_dimension", PilotJson(settings.maxDimension));
        ack.put("fps", PilotJson(settings.framesPerSecond));
        ack.put("room_name", PilotJson(session.roomName));
        ack.put("video_track_name", PilotJson(session.videoTrackName));
        return ack;

    } catch (const PilotException& e) {
        stopLiveRuntime();
        PilotLog::e("Failed to start live: %s", e.what());
        return buildAck(false, PilotString(e.what()));

    } catch (...) {
        stopLiveRuntime();
        PilotLog::e("Failed to start live: unknown error");
        return buildAck(false, "unknown_error");
    }
#endif
}

PilotJson PilotLiveManager::handleUpdate(const PilotString& sessionToken, const PilotJson& payload) {
#ifdef PILOT_NO_LIVEKIT
    PilotLog::w("Live streaming is not supported on this platform (x86)");
    return buildAck(false, "live_not_supported_on_x86");
#else
    if (!m_isLive.load()) {
        return buildAck(false, "Live is not active");
    }

    try {
        LiveSettings requested = LiveSettings::fromPayload(payload);
        PublisherSession session = fetchPublisherSession(sessionToken, requested);
        LiveSettings settings = session.settings;

        {
            std::lock_guard<PilotMutex> lock(m_mutex);
            m_liveSettings = settings;
            if (m_captureProvider) {
                m_captureProvider->onLiveSettingsChanged(settings.maxDimension, settings.framesPerSecond);
            }
        }

        // Wake up capture thread so it picks up new FPS
        m_captureSleepCv.notify_all();

        {
            std::lock_guard<PilotMutex> lock(m_mutex);
            if (m_onLiveModeChanged) {
                m_onLiveModeChanged(true, settings.actionPollIntervalMs);
            }
        }

        PilotLog::i("Live updated (preset=%s, %dx%d@%dfps)",
            settings.presetName.c_str(), settings.maxDimension, settings.maxDimension,
            settings.framesPerSecond);

        PilotJson ack = buildAck(true, "live_updated");
        ack.put("preset", PilotJson(settings.presetName));
        ack.put("max_dimension", PilotJson(settings.maxDimension));
        ack.put("fps", PilotJson(settings.framesPerSecond));
        ack.put("room_name", PilotJson(session.roomName));
        ack.put("video_track_name", PilotJson(session.videoTrackName));
        return ack;

    } catch (const PilotException& e) {
        PilotLog::e("Failed to update live quality: %s", e.what());
        return buildAck(false, PilotString(e.what()));

    } catch (...) {
        PilotLog::e("Failed to update live quality: unknown error");
        return buildAck(false, "unknown_error");
    }
#endif
}

PilotJson PilotLiveManager::handleStop() {
#ifdef PILOT_NO_LIVEKIT
    return buildAck(true, "live_not_supported_on_x86");
#else
    bool wasLive = m_isLive.load();
    stopLiveRuntime();

    {
        std::lock_guard<PilotMutex> lock(m_mutex);
        if (m_onLiveModeChanged) {
            m_onLiveModeChanged(false, 0);
        }
    }

    if (wasLive) {
        PilotLog::i("Live stopped");
    }

    return buildAck(true, wasLive ? "live_stopped" : "live_already_stopped");
#endif
}

PilotJson PilotLiveManager::handleTap(const PilotJson& payload) {
    if (!m_isLive.load()) {
        return buildAck(false, "Live is not active");
    }

    // Win32 does not support touch dispatch
    return buildAck(false, "Touch dispatch is not available on Windows");
}

PilotJson PilotLiveManager::handleLongPress(const PilotJson& payload) {
    if (!m_isLive.load()) {
        return buildAck(false, "Live is not active");
    }

    // Win32 does not support touch dispatch
    return buildAck(false, "Touch dispatch is not available on Windows");
}

void PilotLiveManager::onSessionClosed() {
    stopLiveRuntime();
}

void PilotLiveManager::shutdown() {
    stopLiveRuntime();
}

// ── Private ──

void PilotLiveManager::stopLiveRuntime() {
    m_isLive.store(false);
    m_captureRunning.store(false);
    m_captureSleepCv.notify_all();

    if (m_captureThread.joinable()) {
        m_captureThread.join();
    }

#ifndef PILOT_NO_LIVEKIT
    m_publisher->stop();
#endif

    std::lock_guard<PilotMutex> lock(m_mutex);
    m_liveSettings = LiveSettings::low();
}

PilotLiveManager::PublisherSession PilotLiveManager::fetchPublisherSession(
    const PilotString& sessionToken, const LiveSettings& requested) {

    PilotJson response = m_httpClient.getLivePublisherState(sessionToken);

    if (!response.optBool("configured", false)) {
        PilotString msg = response.optString("status_message", "LiveKit is not configured on the server");
        throw PilotException(msg);
    }

    if (!response.optBool("requested", false)) {
        PilotString msg = response.optString("status_message", "Live request is no longer active");
        throw PilotException(msg);
    }

    PilotString serverUrl = response.optString("server_url");
    PilotString participantToken = response.optString("participant_token");
    PilotString videoTrackName = response.optString("video_track_name");

    if (serverUrl.empty() || participantToken.empty() || videoTrackName.empty()) {
        throw PilotException("Server returned incomplete live credentials");
    }

    PilotString presetName = response.optString("preset");
    if (presetName.empty()) {
        presetName = requested.presetName;
    }
    int maxDimension = clampInt(response.optInt("max_dimension", requested.maxDimension), 240, 1440);
    int fps = clampInt(response.optInt("fps", requested.framesPerSecond), 1, 6);
    int64_t actionPollIntervalMs = static_cast<int64_t>(clampInt(
        response.optInt("action_poll_interval_ms", static_cast<int>(requested.actionPollIntervalMs)), 200, 2000));

    return PublisherSession{
        serverUrl,
        participantToken,
        response.optString("room_name"),
        response.optString("participant_identity"),
        videoTrackName,
        LiveSettings{presetName, maxDimension, fps, actionPollIntervalMs}
    };
}

void PilotLiveManager::frameCaptureLoop() {
    PilotLog::d("Frame capture thread started");

    while (m_captureRunning.load()) {
        int fps;
        PilotScreenCaptureProvider* provider;

        {
            std::lock_guard<PilotMutex> lock(m_mutex);
            fps = m_liveSettings.framesPerSecond;
            provider = m_captureProvider;
        }

        if (!provider) {
            std::unique_lock<PilotMutex> lock(m_captureSleepMutex);
            m_captureSleepCv.wait_for(lock, std::chrono::milliseconds(500),
                [this] { return !m_captureRunning.load(); });
            continue;
        }

        PilotCapturedFrame frame = {};
        bool captured = false;

        try {
            captured = provider->captureFrame(frame);
        } catch (...) {
            PilotLog::e("Screen capture provider threw exception");
        }

        if (captured && frame.data && frame.dataSize > 0) {
            try {
#ifndef PILOT_NO_LIVEKIT
                m_publisher->pushFrame(frame.data, frame.width, frame.height);
#endif
            } catch (const PilotException& e) {
                PilotLog::w("Failed to push live frame: %s", e.what());
            }
        }

        int64_t sleepMs = fps > 0 ? (1000 / fps) : 500;
        std::unique_lock<PilotMutex> lock(m_captureSleepMutex);
        m_captureSleepCv.wait_for(lock, std::chrono::milliseconds(sleepMs),
            [this] { return !m_captureRunning.load(); });
    }

    PilotLog::d("Frame capture thread stopped");
}

PilotJson PilotLiveManager::buildAck(bool ok, const PilotString& status) {
    PilotJson ack = PilotJson::object();
    ack.put("ok", PilotJson(ok));
    ack.put("status", PilotJson(status));
    return ack;
}

} // namespace Pilot
