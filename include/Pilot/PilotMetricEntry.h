#pragma once

#include "Pilot/PilotJson.h"
#include "Pilot/PilotMetricType.h"
#include <cstdint>
#include <chrono>

namespace Pilot {

class PilotMetricEntry {
public:
    PilotMetricEntry(const PilotMetricType& type, double value)
        : m_type(type), m_value(value)
        , m_timestampMs(std::chrono::duration_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now().time_since_epoch()).count()) {}

    PilotMetricEntry(const PilotMetricType& type, double value, int64_t timestampMs)
        : m_type(type), m_value(value), m_timestampMs(timestampMs) {}

    const PilotMetricType& type() const { return m_type; }
    double value() const { return m_value; }
    int64_t timestampMs() const { return m_timestampMs; }

    PilotJson toJson() const {
        return PilotJson::object()
            .put("metric_type", PilotJson(m_type.key()))
            .put("value", PilotJson(m_value))
            .put("client_timestamp", PilotJson(m_timestampMs))
            .put("aggregation", PilotJson(pilotMetricAggregationKey(m_type.aggregation())));
    }

private:
    PilotMetricType m_type;
    double m_value;
    int64_t m_timestampMs;
};

} // namespace Pilot
