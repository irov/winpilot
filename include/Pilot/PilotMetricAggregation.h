#pragma once

namespace Pilot {

enum class PilotMetricAggregation {
    GAUGE,
    COUNTER,
    RATE
};

const char* pilotMetricAggregationKey(PilotMetricAggregation a);

} // namespace Pilot
