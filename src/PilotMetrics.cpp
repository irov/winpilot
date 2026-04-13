#include <Pilot/PilotMetrics.h>
#include <Pilot/PilotLog.h>

namespace Pilot {

void PilotMetrics::addCollector(PilotMetricCollector* collector) {
    std::lock_guard<PilotMutex> lock(m_collectorMutex);
    m_collectors.push_back(collector);
}

void PilotMetrics::removeCollector(PilotMetricCollector* collector) {
    std::lock_guard<PilotMutex> lock(m_collectorMutex);
    m_collectors.erase(
        std::remove(m_collectors.begin(), m_collectors.end(), collector),
        m_collectors.end());
}

void PilotMetrics::record(const PilotMetricType& metricType, double value) {
    bufferEntry(PilotMetricEntry(metricType, value));
}

void PilotMetrics::record(const PilotMetricType& metricType, double value, int64_t timestampMs) {
    bufferEntry(PilotMetricEntry(metricType, value, timestampMs));
}

void PilotMetrics::sample() {
    PilotVector<PilotMetricEntry> collected;

    {
        std::lock_guard<PilotMutex> lock(m_collectorMutex);
        for (auto* collector : m_collectors) {
            try {
                collector->collect(collected);
            } catch (...) {
                PilotLog::e("Metric collector threw exception");
            }
        }
    }

    for (const auto& entry : collected) {
        bufferEntry(entry);
    }
}

PilotVector<PilotMetricEntry> PilotMetrics::drain() {
    std::lock_guard<PilotMutex> lock(m_bufferMutex);
    if (m_buffer.empty()) return {};

    int count = std::min(static_cast<int>(m_buffer.size()), batchSize);
    PilotVector<PilotMetricEntry> chunk(m_buffer.begin(), m_buffer.begin() + count);
    m_buffer.erase(m_buffer.begin(), m_buffer.begin() + count);
    return chunk;
}

void PilotMetrics::requeue(const PilotVector<PilotMetricEntry>& entries) {
    if (entries.empty()) return;
    std::lock_guard<PilotMutex> lock(m_bufferMutex);
    m_buffer.insert(m_buffer.begin(), entries.begin(), entries.end());
    while (static_cast<int>(m_buffer.size()) > bufferSize) {
        m_buffer.pop_back();
    }
}

bool PilotMetrics::hasData() const {
    std::lock_guard<PilotMutex> lock(m_bufferMutex);
    return !m_buffer.empty();
}

void PilotMetrics::clear() {
    {
        std::lock_guard<PilotMutex> lock(m_bufferMutex);
        m_buffer.clear();
    }
    {
        std::lock_guard<PilotMutex> lock(m_collectorMutex);
        m_collectors.clear();
    }
}

void PilotMetrics::bufferEntry(const PilotMetricEntry& entry) {
    std::lock_guard<PilotMutex> lock(m_bufferMutex);
    if (static_cast<int>(m_buffer.size()) >= bufferSize) {
        m_buffer.erase(m_buffer.begin());
    }
    m_buffer.push_back(entry);
}

} // namespace Pilot
