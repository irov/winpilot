#include <Pilot/PilotDefaultMetricCollector.h>
#include <Pilot/PilotLog.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <psapi.h>

namespace Pilot {

PilotDefaultMetricCollector::PilotDefaultMetricCollector() = default;

void PilotDefaultMetricCollector::collect(PilotVector<PilotMetricEntry>& out) {
    collectMemory(out);
    collectCpu(out);
}

void PilotDefaultMetricCollector::collectMemory(PilotVector<PilotMetricEntry>& out) {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        out.emplace_back(PilotMetricType::MEMORY, static_cast<double>(pmc.WorkingSetSize));
    }
}

void PilotDefaultMetricCollector::collectCpu(PilotVector<PilotMetricEntry>& out) {
    FILETIME creationTime, exitTime, kernelTime, userTime;
    if (!GetProcessTimes(GetCurrentProcess(), &creationTime, &exitTime, &kernelTime, &userTime)) {
        return;
    }

    auto toInt64 = [](const FILETIME& ft) -> int64_t {
        return (static_cast<int64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    };

    int64_t kernel = toInt64(kernelTime);
    int64_t user = toInt64(userTime);

    FILETIME nowFt;
    GetSystemTimeAsFileTime(&nowFt);
    int64_t now = toInt64(nowFt);

    if (m_prevCpuTime > 0) {
        int64_t cpuDelta = (kernel - m_prevCpuKernel) + (user - m_prevCpuUser);
        int64_t timeDelta = now - m_prevCpuTime;

        if (timeDelta > 0) {
            double cpuPercent = (static_cast<double>(cpuDelta) / static_cast<double>(timeDelta)) * 100.0;
            out.emplace_back(PilotMetricType::CPU_USAGE, cpuPercent);
        }
    }

    m_prevCpuKernel = kernel;
    m_prevCpuUser = user;
    m_prevCpuTime = now;
}

} // namespace Pilot
