#pragma once

namespace Pilot {

enum class PilotMetricAggregation {
    GAUGE,
    COUNTER,
    RATE
};

inline const char* pilotMetricAggregationKey(PilotMetricAggregation a) {
    switch (a) {
        case PilotMetricAggregation::GAUGE: return "gauge";
        case PilotMetricAggregation::COUNTER: return "counter";
        case PilotMetricAggregation::RATE: return "rate";
    }
    return "gauge";
}

} // namespace Pilot
