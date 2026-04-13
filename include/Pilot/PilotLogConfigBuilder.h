#pragma once

#include "Pilot/PilotLogLevel.h"
#include "Pilot/PilotLogAttributeBuilder.h"

namespace Pilot {

class PilotLogConfigBuilder {
public:
    PilotLogConfigBuilder& setEnabled(bool v);
    PilotLogConfigBuilder& setLogLevel(PilotLogLevel v);
    PilotLogConfigBuilder& setBatchSize(int v);
    PilotLogConfigBuilder& setBufferSize(int v);
    PilotLogConfigBuilder& setAttributes(const PilotLogAttributeBuilder& v);

    bool isEnabled() const;
    PilotLogLevel logLevel() const;
    int batchSize() const;
    int bufferSize() const;
    const PilotLogAttributeBuilder& attributes() const;

private:
    bool m_enabled = true;
    PilotLogLevel m_logLevel = PilotLogLevel::PL_INFO;
    int m_batchSize = 100;
    int m_bufferSize = 1000;
    PilotLogAttributeBuilder m_attributes;
};

} // namespace Pilot
