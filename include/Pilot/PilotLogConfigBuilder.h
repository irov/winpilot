#pragma once

#include "Pilot/PilotLogLevel.h"
#include "Pilot/PilotLogAttributeBuilder.h"

namespace Pilot {

class PilotLogConfigBuilder {
public:
    PilotLogConfigBuilder& setEnabled(bool v) { m_enabled = v; return *this; }
    PilotLogConfigBuilder& setLogLevel(PilotLogLevel v) { m_logLevel = v; return *this; }
    PilotLogConfigBuilder& setBatchSize(int v) { m_batchSize = v; return *this; }
    PilotLogConfigBuilder& setBufferSize(int v) { m_bufferSize = v; return *this; }
    PilotLogConfigBuilder& setAttributes(const PilotLogAttributeBuilder& v) { m_attributes = v; return *this; }

    bool isEnabled() const { return m_enabled; }
    PilotLogLevel logLevel() const { return m_logLevel; }
    int batchSize() const { return m_batchSize; }
    int bufferSize() const { return m_bufferSize; }
    const PilotLogAttributeBuilder& attributes() const { return m_attributes; }

private:
    bool m_enabled = true;
    PilotLogLevel m_logLevel = PilotLogLevel::PL_INFO;
    int m_batchSize = 100;
    int m_bufferSize = 1000;
    PilotLogAttributeBuilder m_attributes;
};

} // namespace Pilot
