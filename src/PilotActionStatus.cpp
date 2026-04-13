#include "Pilot/PilotActionStatus.h"

namespace Pilot {

const char* pilotActionStatusValue(PilotActionStatus s) {
    switch (s) {
        case PilotActionStatus::PENDING: return "pending";
        case PilotActionStatus::DELIVERED: return "delivered";
        case PilotActionStatus::ACKNOWLEDGED: return "acknowledged";
        case PilotActionStatus::UNKNOWN: return "";
    }
    return "";
}

PilotActionStatus pilotActionStatusFromValue(const PilotString& v) {
    if (v == "pending") return PilotActionStatus::PENDING;
    if (v == "delivered") return PilotActionStatus::DELIVERED;
    if (v == "acknowledged") return PilotActionStatus::ACKNOWLEDGED;
    return PilotActionStatus::UNKNOWN;
}

} // namespace Pilot
