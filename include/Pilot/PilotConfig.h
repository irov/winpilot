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
        Builder(const PilotString& baseUrl, const PilotString& apiToken);

        Builder& setDeviceId(const PilotString& v);
        Builder& setDeviceName(const PilotString& v);
        Builder& setPollIntervalMs(int64_t v);
        Builder& setActionPollIntervalMs(int64_t v);
        Builder& setAutoConnect(bool v);
        Builder& setLoggerListener(PilotLoggerListener* v);
        Builder& setSessionListener(PilotSessionListener* v);
        Builder& setActionListener(PilotActionListener* v);
        Builder& setSessionAttributes(const PilotSessionAttributeBuilder& v);
        Builder& setLogConfig(const PilotLogConfigBuilder& v);
        Builder& setMetricConfig(const PilotMetricConfigBuilder& v);

        PilotConfig build() const;

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

    const PilotString& baseUrl() const;
    const PilotString& apiToken() const;
    const PilotString& deviceId() const;
    const PilotString& deviceName() const;
    int64_t pollIntervalMs() const;
    int64_t actionPollIntervalMs() const;
    bool autoConnect() const;
    PilotLoggerListener* loggerListener() const;
    PilotSessionListener* sessionListener() const;
    PilotActionListener* actionListener() const;
    const PilotSessionAttributeBuilder& sessionAttributes() const;
    const PilotLogConfigBuilder& logConfig() const;
    const PilotMetricConfigBuilder& metricConfig() const;

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
