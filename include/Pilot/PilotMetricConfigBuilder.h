#pragma once

#include "Pilot/PilotMetricCollector.h"
#include <cstdint>
#include <algorithm>

namespace Pilot {

class PilotMetricConfigBuilder {
public:
    PilotMetricConfigBuilder& setEnabled(bool v);
    PilotMetricConfigBuilder& setSampleIntervalMs(int64_t ms);
    PilotMetricConfigBuilder& setBufferSize(int v);
    PilotMetricConfigBuilder& setBatchSize(int v);
    PilotMetricConfigBuilder& addCollector(PilotMetricCollector* c);

    bool isEnabled() const;
    int64_t sampleIntervalMs() const;
    int bufferSize() const;
    int batchSize() const;
    const PilotVector<PilotMetricCollector*>& collectors() const;

private:
    bool m_enabled = true;
    int64_t m_sampleIntervalMs = 200;
    int m_bufferSize = 2000;
    int m_batchSize = 200;
    PilotVector<PilotMetricCollector*> m_collectors;
};

} // namespace Pilot
