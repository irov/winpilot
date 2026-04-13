#include "Pilot/PilotMetricConfigBuilder.h"

namespace Pilot {

PilotMetricConfigBuilder& PilotMetricConfigBuilder::setEnabled(bool v) { m_enabled = v; return *this; }

PilotMetricConfigBuilder& PilotMetricConfigBuilder::setSampleIntervalMs(int64_t ms) {
    m_sampleIntervalMs = std::max(int64_t(100), std::min(ms, int64_t(1000)));
    return *this;
}

PilotMetricConfigBuilder& PilotMetricConfigBuilder::setBufferSize(int v) { m_bufferSize = v; return *this; }
PilotMetricConfigBuilder& PilotMetricConfigBuilder::setBatchSize(int v) { m_batchSize = v; return *this; }
PilotMetricConfigBuilder& PilotMetricConfigBuilder::addCollector(PilotMetricCollector* c) { m_collectors.push_back(c); return *this; }

bool PilotMetricConfigBuilder::isEnabled() const { return m_enabled; }
int64_t PilotMetricConfigBuilder::sampleIntervalMs() const { return m_sampleIntervalMs; }
int PilotMetricConfigBuilder::bufferSize() const { return m_bufferSize; }
int PilotMetricConfigBuilder::batchSize() const { return m_batchSize; }
const PilotVector<PilotMetricCollector*>& PilotMetricConfigBuilder::collectors() const { return m_collectors; }

} // namespace Pilot
