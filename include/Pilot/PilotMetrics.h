#pragma once

#include <Pilot/PilotMetricEntry.h>
#include <Pilot/PilotMetricCollector.h>
#include <algorithm>
#include <cstdint>

namespace Pilot {

class PilotMetrics {
public:
    PilotMetrics() = default;

    void addCollector(PilotMetricCollector* collector);
    void removeCollector(PilotMetricCollector* collector);

    void record(const PilotMetricType& metricType, double value);
    void record(const PilotMetricType& metricType, double value, int64_t timestampMs);

    void sample();
    PilotVector<PilotMetricEntry> drain();
    void requeue(const PilotVector<PilotMetricEntry>& entries);
    bool hasData() const;
    void clear();

    int64_t sampleIntervalMs = 200;
    int bufferSize = 2000;
    int batchSize = 200;

private:
    void bufferEntry(const PilotMetricEntry& entry);

    PilotVector<PilotMetricCollector*> m_collectors;
    PilotMutex m_collectorMutex;

    PilotVector<PilotMetricEntry> m_buffer;
    mutable PilotMutex m_bufferMutex;
};

} // namespace Pilot
