#include "Pilot/PilotLogConfigBuilder.h"

namespace Pilot {

PilotLogConfigBuilder& PilotLogConfigBuilder::setEnabled(bool v) { m_enabled = v; return *this; }
PilotLogConfigBuilder& PilotLogConfigBuilder::setLogLevel(PilotLogLevel v) { m_logLevel = v; return *this; }
PilotLogConfigBuilder& PilotLogConfigBuilder::setBatchSize(int v) { m_batchSize = v; return *this; }
PilotLogConfigBuilder& PilotLogConfigBuilder::setBufferSize(int v) { m_bufferSize = v; return *this; }
PilotLogConfigBuilder& PilotLogConfigBuilder::setAttributes(const PilotLogAttributeBuilder& v) { m_attributes = v; return *this; }

bool PilotLogConfigBuilder::isEnabled() const { return m_enabled; }
PilotLogLevel PilotLogConfigBuilder::logLevel() const { return m_logLevel; }
int PilotLogConfigBuilder::batchSize() const { return m_batchSize; }
int PilotLogConfigBuilder::bufferSize() const { return m_bufferSize; }
const PilotLogAttributeBuilder& PilotLogConfigBuilder::attributes() const { return m_attributes; }

} // namespace Pilot
