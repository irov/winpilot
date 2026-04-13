#include <Pilot/PilotSdk.h>
#include <Pilot/PilotHttpClient.h>
#include <Pilot/PilotDefaultMetricCollector.h>
#include <Pilot/PilotLog.h>
#include <Pilot/PilotAction.h>
#include <Pilot/PilotException.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#pragma push_macro("ERROR")
#undef ERROR

#include <chrono>
#include <algorithm>

namespace Pilot {

// ══════════════════════════════════════════════════════
//  Internal instance — holds all mutable state
// ══════════════════════════════════════════════════════

class PilotInstance {
public:
    PilotInstance(const PilotConfig& config);
    ~PilotInstance();

    void startConnection();
    void stopConnection();
    void doShutdown();
    void bufferLog(const PilotLogEntry& entry);
    PilotJson resolveLogAttributes();
    void dispatchQueuedActions();

    PilotConfig config;
    PilotHttpClient httpClient;
    PilotString sessionToken;
    PilotMutex sessionTokenMutex;
    PilotAtomic<PilotSessionStatus> status{PilotSessionStatus::DISCONNECTED};
    PilotUI ui;
    PilotMetrics metrics;
    PilotDefaultMetricCollector defaultMetricCollector;

    PilotVector<PilotActionListener*> actionListeners;
    PilotVector<PilotSessionListener*> sessionListeners;
    PilotMutex listenerMutex;

private:
    PilotAtomic<bool> m_running{false};
    PilotMutex m_sleepMutex;
    PilotConditionVariable m_sleepCv;

    PilotThread m_connectionThread;
    PilotThread m_actionPollThread;
    PilotThread m_metricSampleThread;

    int64_t m_currentActionPollIntervalMs;

    PilotVector<PilotLogEntry> m_logBuffer;
    PilotMutex m_logBufferMutex;
    bool m_logOverflowWarned = false;

    PilotMap<PilotString, PilotJson> m_sessionAttributeCache;

    PilotVector<PilotAction> m_actionQueue;
    PilotMutex m_actionQueueMutex;

    void interruptibleSleep(int64_t ms);

    // Connection
    void connectAndWaitApproval();
    void doConnectAndWaitApproval();
    void onApproved(const PilotString& token);
    void onRejected();

    // Action polling
    void actionPollLoop(const PilotString& token);
    void doPollActions(const PilotString& token);

    // Metric sampling
    void metricSampleLoop();

    // Session attributes
    PilotMap<PilotString, PilotJson> resolveAllSessionAttributes();
    PilotMap<PilotString, PilotJson> resolveChangedSessionAttributes();

    // Log buffering
    PilotVector<PilotLogEntry> drainLogChunk();
    void requeueLogs(const PilotVector<PilotLogEntry>& chunk);
    void flushLogs(const PilotString& token);
    void flushMetrics(const PilotString& token);

    // Reconnection
    void attemptReconnect(const PilotString& token);
    void resetAndRestart();

    // Helpers
    void setStatus(PilotSessionStatus s) { status.store(s); }

    void notifyConnecting();
    void notifyWaitingApproval(const PilotString& reqId);
    void notifySessionStarted(const PilotString& token);
    void notifySessionClosed();
    void notifyRejected();
    void notifyAuthFailed();
    void notifyError(const PilotException& e);
};

// ══════════════════════════════════════════════════════
//  Static singleton state
// ══════════════════════════════════════════════════════

static PilotInstance* s_instance = nullptr;
static PilotMutex s_initMutex;

static PilotInstance* requireInstance() {
    if (!s_instance) {
        PilotLog::e("PilotSdk::initialize() must be called first");
    }
    return s_instance;
}

// ══════════════════════════════════════════════════════
//  PilotSdk static methods
// ══════════════════════════════════════════════════════

void PilotSdk::initialize(const PilotConfig& config) {
    std::lock_guard<PilotMutex> lock(s_initMutex);
    if (s_instance) {
        PilotLog::w("PilotSdk::initialize() called more than once, ignoring");
        return;
    }

    s_instance = new PilotInstance(config);
    PilotLog::i("Pilot SDK initialized (server: %s)", config.baseUrl().c_str());

    if (config.sessionListener()) {
        s_instance->sessionListeners.push_back(config.sessionListener());
    }
    if (config.actionListener()) {
        s_instance->actionListeners.push_back(config.actionListener());
    }

    auto& mc = config.metricConfig();
    if (mc.isEnabled()) {
        s_instance->metrics.addCollector(&s_instance->defaultMetricCollector);
        for (auto* collector : mc.collectors()) {
            s_instance->metrics.addCollector(collector);
        }
        PilotLog::i("Built-in metrics enabled (sample interval: %lldms)", static_cast<long long>(mc.sampleIntervalMs()));
    }

    if (config.autoConnect()) {
        s_instance->startConnection();
    }
}

bool PilotSdk::isInitialized() {
    return s_instance != nullptr;
}

PilotSessionStatus PilotSdk::status() {
    auto* p = s_instance;
    return p ? p->status.load() : PilotSessionStatus::DISCONNECTED;
}

void PilotSdk::addActionListener(PilotActionListener* listener) {
    auto* p = requireInstance();
    if (!p) return;
    std::lock_guard<PilotMutex> lock(p->listenerMutex);
    p->actionListeners.push_back(listener);
}

void PilotSdk::removeActionListener(PilotActionListener* listener) {
    auto* p = s_instance;
    if (!p) return;
    std::lock_guard<PilotMutex> lock(p->listenerMutex);
    auto& v = p->actionListeners;
    v.erase(std::remove(v.begin(), v.end(), listener), v.end());
}

void PilotSdk::addSessionListener(PilotSessionListener* listener) {
    auto* p = requireInstance();
    if (!p) return;
    std::lock_guard<PilotMutex> lock(p->listenerMutex);
    p->sessionListeners.push_back(listener);
}

void PilotSdk::removeSessionListener(PilotSessionListener* listener) {
    auto* p = s_instance;
    if (!p) return;
    std::lock_guard<PilotMutex> lock(p->listenerMutex);
    auto& v = p->sessionListeners;
    v.erase(std::remove(v.begin(), v.end(), listener), v.end());
}

PilotUI& PilotSdk::getUI() {
    return requireInstance()->ui;
}

PilotMetrics& PilotSdk::getMetrics() {
    return requireInstance()->metrics;
}

void PilotSdk::connect() {
    auto* p = requireInstance();
    if (p) p->startConnection();
}

void PilotSdk::disconnect() {
    auto* p = s_instance;
    if (p) p->stopConnection();
}

void PilotSdk::update() {
    auto* p = s_instance;
    if (!p) return;
    p->ui.pollValues();
    p->dispatchQueuedActions();
}

void PilotSdk::log(PilotLogLevel level, const PilotString& message) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(PilotLogEntry(level, message, PilotString(), PilotString(), {}, p->resolveLogAttributes()));
}

void PilotSdk::log(PilotLogLevel level, const PilotString& message,
                    const PilotString& category, const PilotString& thread) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(PilotLogEntry(level, message, category, thread, {}, p->resolveLogAttributes()));
}

void PilotSdk::log(PilotLogLevel level, const PilotString& message,
                    const PilotMap<PilotString, PilotJson>& metadata) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(PilotLogEntry(level, message, PilotString(), PilotString(), metadata, p->resolveLogAttributes()));
}

void PilotSdk::log(PilotLogLevel level, const PilotString& message,
                    const PilotString& category, const PilotString& thread,
                    const PilotMap<PilotString, PilotJson>& metadata) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(PilotLogEntry(level, message, category, thread, metadata, p->resolveLogAttributes()));
}

void PilotSdk::log(const PilotLogEntry& entry) {
    auto* p = s_instance;
    if (!p || !p->config.logConfig().isEnabled()) return;
    p->bufferLog(entry);
}

static void bufferStructuredLog(PilotInstance* p, const PilotString& kind,
                                const PilotString& message, const PilotString& category,
                                const PilotMap<PilotString, PilotJson>& metadata,
                                int64_t clientTimestampMs) {
    if (!p || !p->config.logConfig().isEnabled()) return;

    PilotString resolvedCategory;
    if (category.empty()) {
        resolvedCategory = kind;
    } else if (category == kind || category.substr(0, kind.size() + 1) == kind + PilotString("_")) {
        resolvedCategory = category;
    } else {
        resolvedCategory = kind + PilotString("_") + category;
    }

    PilotMap<PilotString, PilotJson> merged = metadata;
    merged["pilot_kind"] = PilotJson(kind);

    p->bufferLog(PilotLogEntry(
        PilotLogLevel::INFO, message, resolvedCategory, PilotString(),
        merged, p->resolveLogAttributes(), clientTimestampMs));
}

static int64_t nowMs() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

void PilotSdk::event(const PilotString& message) {
    bufferStructuredLog(s_instance, "event", message, PilotString(), {}, nowMs());
}

void PilotSdk::event(const PilotString& message, const PilotMap<PilotString, PilotJson>& metadata) {
    bufferStructuredLog(s_instance, "event", message, PilotString(), metadata, nowMs());
}

void PilotSdk::event(const PilotString& message, const PilotString& category,
                      const PilotMap<PilotString, PilotJson>& metadata) {
    bufferStructuredLog(s_instance, "event", message, category, metadata, nowMs());
}

void PilotSdk::revenue(const PilotString& message) {
    bufferStructuredLog(s_instance, "revenue", message, PilotString(), {}, nowMs());
}

void PilotSdk::revenue(const PilotString& message, const PilotMap<PilotString, PilotJson>& metadata) {
    bufferStructuredLog(s_instance, "revenue", message, PilotString(), metadata, nowMs());
}

void PilotSdk::revenue(const PilotString& message, const PilotString& category,
                        const PilotMap<PilotString, PilotJson>& metadata) {
    bufferStructuredLog(s_instance, "revenue", message, category, metadata, nowMs());
}

void PilotSdk::changeScreen(const PilotString& screenType, const PilotString& screenName) {
    PilotMap<PilotString, PilotJson> metadata;
    metadata["pilot_command"] = PilotJson("change_screen");
    metadata["pilot_slice_type"] = PilotJson("screen");
    metadata["pilot_slice_name"] = PilotJson(screenName);
    metadata["screen_type"] = PilotJson(screenType);
    metadata["screen_name"] = PilotJson(screenName);
    bufferStructuredLog(s_instance, "event", "change_screen", "change_screen", metadata, nowMs());
}

void PilotSdk::acknowledgeAction(const PilotString& actionId, const PilotJson& ackPayload) {
    auto* p = s_instance;
    if (!p) return;
    PilotString token;
    {
        std::lock_guard<PilotMutex> lock(p->sessionTokenMutex);
        token = p->sessionToken;
    }
    if (token.empty()) return;
    try {
        p->httpClient.acknowledgeAction(token, actionId, ackPayload);
    } catch (const PilotException& e) {
        PilotLog::e("Failed to acknowledge action: %s", e.what());
    }
}

void PilotSdk::shutdown() {
    std::lock_guard<PilotMutex> lock(s_initMutex);
    auto* p = s_instance;
    s_instance = nullptr;
    if (p) {
        p->doShutdown();
        delete p;
    }
}

// ══════════════════════════════════════════════════════
//  PilotInstance implementation
// ══════════════════════════════════════════════════════

PilotInstance::PilotInstance(const PilotConfig& config)
    : config(config)
    , httpClient(config.baseUrl(), config.apiToken())
    , m_currentActionPollIntervalMs(config.actionPollIntervalMs()) {
    PilotLog::setLevel(config.logConfig().logLevel());
    PilotLog::setLoggerListener(config.loggerListener());

    auto& mc = config.metricConfig();
    if (mc.isEnabled()) {
        metrics.sampleIntervalMs = mc.sampleIntervalMs();
        metrics.bufferSize = mc.bufferSize();
        metrics.batchSize = mc.batchSize();
    }
}

PilotInstance::~PilotInstance() {
    doShutdown();
}

void PilotInstance::startConnection() {
    if (m_running.exchange(true)) {
        PilotLog::w("Already connecting/connected");
        return;
    }

    m_connectionThread = PilotThread([this] { connectAndWaitApproval(); });
}

void PilotInstance::stopConnection() {
    if (!m_running.exchange(false)) return;

    // Wake up sleeping threads
    m_sleepCv.notify_all();

    // Wait for threads to finish
    if (m_connectionThread.joinable()) m_connectionThread.join();
    if (m_actionPollThread.joinable()) m_actionPollThread.join();
    if (m_metricSampleThread.joinable()) m_metricSampleThread.join();

    PilotString token;
    {
        std::lock_guard<PilotMutex> lock(sessionTokenMutex);
        token = sessionToken;
        sessionToken.clear();
    }

    if (!token.empty()) {
        try {
            flushLogs(token);
            flushMetrics(token);
            httpClient.closeSession(token);
        } catch (...) {}
        setStatus(PilotSessionStatus::CLOSED);
        notifySessionClosed();
    } else {
        setStatus(PilotSessionStatus::DISCONNECTED);
    }
}

void PilotInstance::doShutdown() {
    PilotLog::i("Shutting down Pilot SDK");
    stopConnection();
    httpClient.shutdown();
    {
        std::lock_guard<PilotMutex> lock(listenerMutex);
        actionListeners.clear();
        sessionListeners.clear();
    }
    {
        std::lock_guard<PilotMutex> lock(m_logBufferMutex);
        m_logBuffer.clear();
    }
    metrics.clear();
}

void PilotInstance::bufferLog(const PilotLogEntry& entry) {
    std::lock_guard<PilotMutex> lock(m_logBufferMutex);
    if (static_cast<int>(m_logBuffer.size()) >= config.logConfig().bufferSize()) {
        m_logBuffer.erase(m_logBuffer.begin());
        if (!m_logOverflowWarned) {
            m_logOverflowWarned = true;
            PilotLog::w("Log buffer overflow (%d), dropping oldest entries", config.logConfig().bufferSize());
        }
    }
    m_logBuffer.push_back(entry);
}

PilotJson PilotInstance::resolveLogAttributes() {
    const auto& builder = config.logConfig().attributes();
    const auto& staticAttrs = builder.staticAttributes();
    const auto& dynamicAttrs = builder.dynamicAttributes();

    if (staticAttrs.empty() && dynamicAttrs.empty()) return PilotJson();

    PilotJson attributes = PilotJson::object();
    for (const auto& kv : staticAttrs) {
        attributes.put(kv.first, PilotJson(kv.second));
    }
    for (const auto& kv : dynamicAttrs) {
        try {
            PilotString val = kv.second->getValue();
            attributes.put(kv.first, PilotJson(val));
        } catch (...) {}
    }

    return attributes.objectSize() == 0 ? PilotJson() : attributes;
}

void PilotInstance::dispatchQueuedActions() {
    PilotVector<PilotAction> actions;
    {
        std::lock_guard<PilotMutex> lock(m_actionQueueMutex);
        actions.swap(m_actionQueue);
    }

    for (const auto& action : actions) {
        bool handled = false;
        try {
            handled = ui.dispatchAction(action);
        } catch (...) {
            PilotLog::e("Widget callback threw exception");
        }

        if (!handled) {
            std::lock_guard<PilotMutex> lock(listenerMutex);
            for (auto* listener : actionListeners) {
                try {
                    listener->onPilotActionReceived(action);
                } catch (...) {
                    PilotLog::e("Action listener threw exception");
                }
            }
        }
    }
}

void PilotInstance::interruptibleSleep(int64_t ms) {
    std::unique_lock<PilotMutex> lock(m_sleepMutex);
    m_sleepCv.wait_for(lock, std::chrono::milliseconds(ms), [this] { return !m_running.load(); });
}

// ── Connection logic ──

void PilotInstance::connectAndWaitApproval() {
    int64_t retryDelayMs = 2000;
    const int64_t maxRetryDelayMs = 30000;
    int retryCount = 0;

    while (m_running.load()) {
        try {
            doConnectAndWaitApproval();
            return;
        } catch (const PilotException& e) {
            if (e.isUnauthorized()) {
                PilotLog::e("Authentication failed: %s", e.what());
                setStatus(PilotSessionStatus::AUTH_FAILED);
                m_running.store(false);
                notifyAuthFailed();
                return;
            }
            if (!e.isNetworkError()) {
                PilotLog::e("Server error, stopping connection: %s", e.what());
                setStatus(PilotSessionStatus::ERROR);
                m_running.store(false);
                notifyError(e);
                return;
            }

            retryCount++;
            PilotLog::w("Connection attempt %d failed (network): %s, retrying in %lldms",
                        retryCount, e.what(), static_cast<long long>(retryDelayMs));
            setStatus(PilotSessionStatus::CONNECTING);
            interruptibleSleep(retryDelayMs);
            retryDelayMs = std::min(retryDelayMs * 2, maxRetryDelayMs);
        }
    }
}

void PilotInstance::doConnectAndWaitApproval() {
    setStatus(PilotSessionStatus::CONNECTING);
    notifyConnecting();

    PilotString deviceId = config.deviceId();
    PilotString deviceName = config.deviceName();

    if (deviceId.empty()) {
        char compName[256] = {};
        DWORD size = sizeof(compName);
        GetComputerNameA(compName, &size);
        deviceId = compName;
    }
    if (deviceName.empty()) {
        char compName[256] = {};
        DWORD size = sizeof(compName);
        GetComputerNameA(compName, &size);
        deviceName = PilotString(compName) + PilotString(" (Windows)");
    }

    auto resp = httpClient.connect(deviceId, deviceName, resolveAllSessionAttributes());

    PilotLog::i("Connect request sent, request_id=%s, status=%s",
                resp.requestId().c_str(), resp.status().c_str());

    if (resp.isApproved() && !resp.sessionToken().empty()) {
        onApproved(resp.sessionToken());
        return;
    }

    if (resp.isRejected()) {
        onRejected();
        return;
    }

    setStatus(PilotSessionStatus::WAITING_APPROVAL);
    notifyWaitingApproval(resp.requestId());

    while (m_running.load()) {
        interruptibleSleep(config.pollIntervalMs());
        if (!m_running.load()) return;

        auto pollResp = httpClient.pollStatus(resp.requestId());

        if (pollResp.isApproved() && !pollResp.sessionToken().empty()) {
            onApproved(pollResp.sessionToken());
            return;
        }

        if (pollResp.isRejected()) {
            onRejected();
            return;
        }
    }
}

void PilotInstance::onApproved(const PilotString& token) {
    {
        std::lock_guard<PilotMutex> lock(sessionTokenMutex);
        sessionToken = token;
    }
    setStatus(PilotSessionStatus::ACTIVE);
    PilotLog::i("Session approved and active");
    notifySessionStarted(token);

    // Submit initial UI if available
    if (ui.hasTabs()) {
        PilotJson snapshot = ui.toJson();
        try {
            httpClient.submitPanel(token, snapshot);
            ui.markSent();
            PilotLog::d("Initial UI submitted (revision=%d)", ui.getRevision());
        } catch (const PilotException& e) {
            PilotLog::e("Failed to submit initial UI: %s", e.what());
            notifyError(e);
        }
    }

    // Start action polling thread
    m_actionPollThread = PilotThread([this, token] { actionPollLoop(token); });

    // Start metric sampling thread
    if (config.metricConfig().isEnabled()) {
        m_metricSampleThread = PilotThread([this] { metricSampleLoop(); });
    }
}

void PilotInstance::onRejected() {
    PilotLog::w("Connection request rejected");
    setStatus(PilotSessionStatus::REJECTED);
    m_running.store(false);
    notifyRejected();
}

// ── Action polling ──

void PilotInstance::actionPollLoop(const PilotString& token) {
    while (m_running.load()) {
        doPollActions(token);
        interruptibleSleep(m_currentActionPollIntervalMs);
    }
}

void PilotInstance::doPollActions(const PilotString& token) {
    try {
        auto changedAttrs = resolveChangedSessionAttributes();
        auto logChunk = drainLogChunk();
        auto metricChunk = metrics.drain();

        // Snapshot and submit UI if changed
        if (ui.hasUnsent()) {
            PilotJson uiSnapshot = ui.toJson();
            try {
                httpClient.submitPanel(token, uiSnapshot);
                ui.markSent();
                PilotLog::d("UI submitted (revision=%d)", ui.getRevision());
            } catch (const PilotException& e) {
                PilotLog::e("Failed to submit UI: %s", e.what());
                notifyError(e);
            }
        }

        const PilotMap<PilotString, PilotJson>* changedPtr = changedAttrs.empty() ? nullptr : &changedAttrs;
        PilotJson json = httpClient.pollActions(token, changedPtr, logChunk, metricChunk);
        PilotJson actionsArr = json.optArray("actions");

        if (actionsArr.length() > 0) {
            std::lock_guard<PilotMutex> lock(m_actionQueueMutex);
            for (int i = 0; i < actionsArr.length(); ++i) {
                PilotJson actionJson = actionsArr.at(i);
                if (!actionJson.isNull()) {
                    m_actionQueue.push_back(PilotAction::fromJson(actionJson));
                }
            }
        }

        if (!logChunk.empty()) {
            m_logOverflowWarned = false;
        }
    } catch (const PilotException& e) {
        if (e.isNetworkError()) {
            PilotLog::w("Network error during action poll, will retry");
        } else {
            PilotLog::w("Action poll failed with server error, attempting reconnect");
            attemptReconnect(token);
        }
    }
}

// ── Metric sampling ──

void PilotInstance::metricSampleLoop() {
    while (m_running.load()) {
        metrics.sample();
        interruptibleSleep(metrics.sampleIntervalMs);
    }
}

// ── Session attributes ──

PilotMap<PilotString, PilotJson> PilotInstance::resolveAllSessionAttributes() {
    const auto& builder = config.sessionAttributes();
    PilotMap<PilotString, PilotJson> merged = builder.staticAttributes();

    for (const auto& kv : builder.dynamicAttributes()) {
        try {
            PilotString val = kv.second->getValue();
            PilotJson resolved(val);
            merged[kv.first] = resolved;
            m_sessionAttributeCache[kv.first] = resolved;
        } catch (...) {
            PilotLog::e("Session attribute provider failed: %s", kv.first.c_str());
        }
    }

    return merged;
}

PilotMap<PilotString, PilotJson> PilotInstance::resolveChangedSessionAttributes() {
    const auto& dynamicAttrs = config.sessionAttributes().dynamicAttributes();
    if (dynamicAttrs.empty()) return {};

    PilotMap<PilotString, PilotJson> changed;

    for (const auto& kv : dynamicAttrs) {
        try {
            PilotString val = kv.second->getValue();
            PilotJson resolved(val);
            auto cached = m_sessionAttributeCache.find(kv.first);
            if (cached == m_sessionAttributeCache.end() || cached->second.asString() != val) {
                m_sessionAttributeCache[kv.first] = resolved;
                changed[kv.first] = resolved;
            }
        } catch (...) {
            PilotLog::e("Session attribute provider failed: %s", kv.first.c_str());
        }
    }

    return changed;
}

// ── Log buffering ──

PilotVector<PilotLogEntry> PilotInstance::drainLogChunk() {
    std::lock_guard<PilotMutex> lock(m_logBufferMutex);
    if (m_logBuffer.empty()) return {};

    int count = std::min(static_cast<int>(m_logBuffer.size()), config.logConfig().batchSize());
    PilotVector<PilotLogEntry> chunk(m_logBuffer.begin(), m_logBuffer.begin() + count);
    m_logBuffer.erase(m_logBuffer.begin(), m_logBuffer.begin() + count);
    return chunk;
}

void PilotInstance::requeueLogs(const PilotVector<PilotLogEntry>& chunk) {
    if (chunk.empty()) return;
    std::lock_guard<PilotMutex> lock(m_logBufferMutex);
    m_logBuffer.insert(m_logBuffer.begin(), chunk.begin(), chunk.end());
    while (static_cast<int>(m_logBuffer.size()) > config.logConfig().bufferSize()) {
        m_logBuffer.pop_back();
    }
}

void PilotInstance::flushLogs(const PilotString& token) {
    auto chunk = drainLogChunk();
    if (chunk.empty()) return;
    try {
        httpClient.sendLogs(token, chunk);
    } catch (const PilotException&) {
        requeueLogs(chunk);
    }
}

void PilotInstance::flushMetrics(const PilotString& token) {
    auto chunk = metrics.drain();
    if (chunk.empty()) return;
    try {
        httpClient.sendMetrics(token, chunk);
    } catch (const PilotException&) {
        metrics.requeue(chunk);
    }
}

// ── Reconnection ──

void PilotInstance::attemptReconnect(const PilotString& token) {
    setStatus(PilotSessionStatus::CONNECTING);

    int64_t retryDelay = 2000;
    const int64_t maxDelay = 30000;

    while (m_running.load()) {
        interruptibleSleep(retryDelay);
        if (!m_running.load()) return;

        try {
            httpClient.pollActions(token, nullptr, {}, {});
            PilotLog::i("Reconnect successful, session still active");
            setStatus(PilotSessionStatus::ACTIVE);
            return;
        } catch (const PilotException& e) {
            if (e.isNetworkError()) {
                PilotLog::w("Reconnect: no network, retrying in %lldms", static_cast<long long>(retryDelay));
                retryDelay = std::min(retryDelay * 2, maxDelay);
            } else {
                PilotLog::w("Session lost (server error), starting fresh connection");
                resetAndRestart();
                return;
            }
        }
    }
}

void PilotInstance::resetAndRestart() {
    m_running.store(false);
    {
        std::lock_guard<PilotMutex> lock(sessionTokenMutex);
        sessionToken.clear();
    }
    setStatus(PilotSessionStatus::DISCONNECTED);
    notifySessionClosed();
    startConnection();
}

// ── Listener notifications ──

void PilotInstance::notifyConnecting() {
    std::lock_guard<PilotMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onPilotSessionConnecting();
}

void PilotInstance::notifyWaitingApproval(const PilotString& reqId) {
    std::lock_guard<PilotMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onPilotSessionWaitingApproval(reqId);
}

void PilotInstance::notifySessionStarted(const PilotString& token) {
    std::lock_guard<PilotMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onPilotSessionStarted(token);
}

void PilotInstance::notifySessionClosed() {
    std::lock_guard<PilotMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onPilotSessionClosed();
}

void PilotInstance::notifyRejected() {
    std::lock_guard<PilotMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onPilotSessionRejected();
}

void PilotInstance::notifyAuthFailed() {
    std::lock_guard<PilotMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onPilotSessionAuthFailed();
}

void PilotInstance::notifyError(const PilotException& e) {
    std::lock_guard<PilotMutex> lock(listenerMutex);
    for (auto* l : sessionListeners) l->onPilotSessionError(e);
}

} // namespace Pilot

#pragma pop_macro("ERROR")
