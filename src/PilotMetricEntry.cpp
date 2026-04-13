#include "Pilot/PilotMetricEntry.h"

namespace Pilot {

PilotMetricEntry::PilotMetricEntry(const PilotMetricType& type, double value)
    : m_type(type), m_value(value)
    , m_timestampMs(std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now().time_since_epoch()).count()) {
}

PilotMetricEntry::PilotMetricEntry(const PilotMetricType& type, double value, int64_t timestampMs)
    : m_type(type), m_value(value), m_timestampMs(timestampMs) {
}

const PilotMetricType& PilotMetricEntry::type() const { return m_type; }
double PilotMetricEntry::value() const { return m_value; }
int64_t PilotMetricEntry::timestampMs() const { return m_timestampMs; }

PilotJson PilotMetricEntry::toJson() const {
    return PilotJson::object()
        .put("metric_type", PilotJson(m_type.key()))
        .put("value", PilotJson(m_value))
        .put("client_timestamp", PilotJson(m_timestampMs))
        .put("aggregation", PilotJson(pilotMetricAggregationKey(m_type.aggregation())));
}

} // namespace Pilot
