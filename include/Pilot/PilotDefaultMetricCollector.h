#pragma once

#include <Pilot/PilotMetricCollector.h>
#include <Pilot/PilotMetricEntry.h>
#include <Pilot/PilotMetricType.h>
#include <cstdint>

namespace Pilot {

class PilotDefaultMetricCollector : public PilotMetricCollector {
public:
    PilotDefaultMetricCollector();
    void collect(PilotVector<PilotMetricEntry>& out) override;

private:
    void collectMemory(PilotVector<PilotMetricEntry>& out);
    void collectCpu(PilotVector<PilotMetricEntry>& out);

    int64_t m_prevCpuKernel = 0;
    int64_t m_prevCpuUser = 0;
    int64_t m_prevCpuTime = 0;
};

} // namespace Pilot
