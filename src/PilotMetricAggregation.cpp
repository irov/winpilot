#include "Pilot/PilotMetricAggregation.h"

namespace Pilot {

const char* pilotMetricAggregationKey(PilotMetricAggregation a) {
    switch (a) {
        case PilotMetricAggregation::GAUGE: return "gauge";
        case PilotMetricAggregation::COUNTER: return "counter";
        case PilotMetricAggregation::RATE: return "rate";
    }
    return "gauge";
}

} // namespace Pilot
