#pragma once

#include "Pilot/PilotSessionAttributeBuilder.h"
#include "Pilot/PilotLogConfigBuilder.h"
#include "Pilot/PilotMetricConfigBuilder.h"
#include "Pilot/PilotSessionListener.h"
#include "Pilot/PilotActionListener.h"
#include "Pilot/PilotLoggerListener.h"

namespace Pilot {

class PilotConfig {
public:
    class Builder {
    public:
        Builder(const PilotString& baseUrl, const PilotString& apiToken)
            : m_baseUrl(baseUrl), m_apiToken(apiToken) {
            // trim trailing slash
            while (!m_baseUrl.empty() && m_baseUrl.back() == '/') {
                m_baseUrl.pop_back();
            }
        }

        Builder& setDeviceId(const PilotString& v) { m_deviceId = v; return *this; }
        Builder& setDeviceName(const PilotString& v) { m_deviceName = v; return *this; }
        Builder& setPollIntervalMs(int64_t v) { m_pollIntervalMs = v; return *this; }
        Builder& setActionPollIntervalMs(int64_t v) { m_actionPollIntervalMs = v; return *this; }
        Builder& setAutoConnect(bool v) { m_autoConnect = v; return *this; }
        Builder& setLoggerListener(PilotLoggerListener* v) { m_loggerListener = v; return *this; }
        Builder& setSessionListener(PilotSessionListener* v) { m_sessionListener = v; return *this; }
        Builder& setActionListener(PilotActionListener* v) { m_actionListener = v; return *this; }
        Builder& setSessionAttributes(const PilotSessionAttributeBuilder& v) { m_sessionAttributes = v; return *this; }
        Builder& setLogConfig(const PilotLogConfigBuilder& v) { m_logConfig = v; return *this; }
        Builder& setMetricConfig(const PilotMetricConfigBuilder& v) { m_metricConfig = v; return *this; }

        PilotConfig build() const {
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

    private:
        PilotString m_baseUrl;
        PilotString m_apiToken;
        PilotString m_deviceId;
        PilotString m_deviceName;
        int64_t m_pollIntervalMs = 10000;
        int64_t m_actionPollIntervalMs = 2000;
        bool m_autoConnect = true;
        PilotLoggerListener* m_loggerListener = nullptr;
        PilotSessionListener* m_sessionListener = nullptr;
        PilotActionListener* m_actionListener = nullptr;
        PilotSessionAttributeBuilder m_sessionAttributes;
        PilotLogConfigBuilder m_logConfig;
        PilotMetricConfigBuilder m_metricConfig;
    };

    const PilotString& baseUrl() const { return m_baseUrl; }
    const PilotString& apiToken() const { return m_apiToken; }
    const PilotString& deviceId() const { return m_deviceId; }
    const PilotString& deviceName() const { return m_deviceName; }
    int64_t pollIntervalMs() const { return m_pollIntervalMs; }
    int64_t actionPollIntervalMs() const { return m_actionPollIntervalMs; }
    bool autoConnect() const { return m_autoConnect; }
    PilotLoggerListener* loggerListener() const { return m_loggerListener; }
    PilotSessionListener* sessionListener() const { return m_sessionListener; }
    PilotActionListener* actionListener() const { return m_actionListener; }
    const PilotSessionAttributeBuilder& sessionAttributes() const { return m_sessionAttributes; }
    const PilotLogConfigBuilder& logConfig() const { return m_logConfig; }
    const PilotMetricConfigBuilder& metricConfig() const { return m_metricConfig; }

private:
    PilotConfig() = default;

    PilotString m_baseUrl;
    PilotString m_apiToken;
    PilotString m_deviceId;
    PilotString m_deviceName;
    int64_t m_pollIntervalMs = 10000;
    int64_t m_actionPollIntervalMs = 2000;
    bool m_autoConnect = true;
    PilotLoggerListener* m_loggerListener = nullptr;
    PilotSessionListener* m_sessionListener = nullptr;
    PilotActionListener* m_actionListener = nullptr;
    PilotSessionAttributeBuilder m_sessionAttributes;
    PilotLogConfigBuilder m_logConfig;
    PilotMetricConfigBuilder m_metricConfig;
};

} // namespace Pilot
