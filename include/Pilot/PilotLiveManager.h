#pragma once

#include "Pilot/PilotTypes.h"
#include "Pilot/PilotJson.h"

namespace Pilot {

class PilotHttpClient;
class PilotScreenCaptureProvider;
class PilotLiveKitPublisher;

class PilotLiveManager {
public:
    PilotLiveManager(PilotHttpClient& httpClient);
    ~PilotLiveManager();

    void setScreenCaptureProvider(PilotScreenCaptureProvider* provider);
    void setOnLiveModeChanged(PilotFunction<void(bool, int64_t)> callback);

    PilotJson handleStart(const PilotString& sessionToken, const PilotJson& payload);
    PilotJson handleUpdate(const PilotString& sessionToken, const PilotJson& payload);
    PilotJson handleStop();
    PilotJson handleTap(const PilotJson& payload);
    PilotJson handleLongPress(const PilotJson& payload);

    bool isLive() const;
    void onSessionClosed();
    void shutdown();

private:
    struct LiveSettings {
        PilotString presetName;
        int maxDimension;
        int framesPerSecond;
        int64_t actionPollIntervalMs;

        static LiveSettings low();
        static LiveSettings balanced();
        static LiveSettings high();
        static LiveSettings fromPayload(const PilotJson& payload);
    };

    struct PublisherSession {
        PilotString serverUrl;
        PilotString participantToken;
        PilotString roomName;
        PilotString participantIdentity;
        PilotString videoTrackName;
        LiveSettings settings;
    };

    void stopLiveRuntime();
    PublisherSession fetchPublisherSession(const PilotString& sessionToken, const LiveSettings& requested);
    void frameCaptureLoop();

    static PilotJson buildAck(bool ok, const PilotString& status);

    PilotHttpClient& m_httpClient;
    PilotScreenCaptureProvider* m_captureProvider;
    PilotFunction<void(bool, int64_t)> m_onLiveModeChanged;

    PilotUniquePtr<PilotLiveKitPublisher> m_publisher;

    mutable PilotMutex m_mutex;
    PilotAtomic<bool> m_isLive{false};
    LiveSettings m_liveSettings;

    PilotAtomic<bool> m_captureRunning{false};
    PilotThread m_captureThread;
    PilotMutex m_captureSleepMutex;
    PilotConditionVariable m_captureSleepCv;
};

} // namespace Pilot
