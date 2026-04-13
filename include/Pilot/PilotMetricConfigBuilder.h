#pragma once

#include "Pilot/PilotMetricCollector.h"
#include <cstdint>
#include <algorithm>

namespace Pilot {

class PilotMetricConfigBuilder {
public:
    PilotMetricConfigBuilder& setEnabled(bool v) { m_enabled = v; return *this; }
    PilotMetricConfigBuilder& setSampleIntervalMs(int64_t ms) {
        m_sampleIntervalMs = std::max(int64_t(100), std::min(ms, int64_t(1000)));
        return *this;
    }
    PilotMetricConfigBuilder& setBufferSize(int v) { m_bufferSize = v; return *this; }
    PilotMetricConfigBuilder& setBatchSize(int v) { m_batchSize = v; return *this; }
    PilotMetricConfigBuilder& addCollector(PilotMetricCollector* c) { m_collectors.push_back(c); return *this; }

    bool isEnabled() const { return m_enabled; }
    int64_t sampleIntervalMs() const { return m_sampleIntervalMs; }
    int bufferSize() const { return m_bufferSize; }
    int batchSize() const { return m_batchSize; }
    const PilotVector<PilotMetricCollector*>& collectors() const { return m_collectors; }

private:
    bool m_enabled = true;
    int64_t m_sampleIntervalMs = 200;
    int m_bufferSize = 2000;
    int m_batchSize = 200;
    PilotVector<PilotMetricCollector*> m_collectors;
};

} // namespace Pilot
