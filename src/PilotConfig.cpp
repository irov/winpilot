#include "Pilot/PilotConfig.h"

namespace Pilot {

// ── PilotConfig::Builder ──

PilotConfig::Builder::Builder(const PilotString& baseUrl, const PilotString& apiToken)
    : m_baseUrl(baseUrl), m_apiToken(apiToken) {
    while (!m_baseUrl.empty() && m_baseUrl.back() == '/') {
        m_baseUrl.pop_back();
    }
}

PilotConfig::Builder& PilotConfig::Builder::setDeviceId(const PilotString& v) { m_deviceId = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setDeviceName(const PilotString& v) { m_deviceName = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setPollIntervalMs(int64_t v) { m_pollIntervalMs = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setActionPollIntervalMs(int64_t v) { m_actionPollIntervalMs = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setAutoConnect(bool v) { m_autoConnect = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setLoggerListener(PilotLoggerListener* v) { m_loggerListener = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setSessionListener(PilotSessionListener* v) { m_sessionListener = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setActionListener(PilotActionListener* v) { m_actionListener = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setSessionAttributes(const PilotSessionAttributeBuilder& v) { m_sessionAttributes = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setLogConfig(const PilotLogConfigBuilder& v) { m_logConfig = v; return *this; }
PilotConfig::Builder& PilotConfig::Builder::setMetricConfig(const PilotMetricConfigBuilder& v) { m_metricConfig = v; return *this; }

PilotConfig PilotConfig::Builder::build() const {
    PilotConfig c;
    c.m_baseUrl = m_baseUrl;
    c.m_apiToken = m_apiToken;
    c.m_deviceId = m_deviceId;
    c.m_deviceName = m_deviceName;
    c.m_pollIntervalMs = m_pollIntervalMs;
    c.m_actionPollIntervalMs = m_actionPollIntervalMs;
    c.m_autoConnect = m_autoConnect;
    c.m_loggerListener = m_loggerListener;
    c.m_sessionListener = m_sessionListener;
    c.m_actionListener = m_actionListener;
    c.m_sessionAttributes = m_sessionAttributes;
    c.m_logConfig = m_logConfig;
    c.m_metricConfig = m_metricConfig;
    return c;
}

// ── PilotConfig ──

const PilotString& PilotConfig::baseUrl() const { return m_baseUrl; }
const PilotString& PilotConfig::apiToken() const { return m_apiToken; }
const PilotString& PilotConfig::deviceId() const { return m_deviceId; }
const PilotString& PilotConfig::deviceName() const { return m_deviceName; }
int64_t PilotConfig::pollIntervalMs() const { return m_pollIntervalMs; }
int64_t PilotConfig::actionPollIntervalMs() const { return m_actionPollIntervalMs; }
bool PilotConfig::autoConnect() const { return m_autoConnect; }
PilotLoggerListener* PilotConfig::loggerListener() const { return m_loggerListener; }
PilotSessionListener* PilotConfig::sessionListener() const { return m_sessionListener; }
PilotActionListener* PilotConfig::actionListener() const { return m_actionListener; }
const PilotSessionAttributeBuilder& PilotConfig::sessionAttributes() const { return m_sessionAttributes; }
const PilotLogConfigBuilder& PilotConfig::logConfig() const { return m_logConfig; }
const PilotMetricConfigBuilder& PilotConfig::metricConfig() const { return m_metricConfig; }

} // namespace Pilot
