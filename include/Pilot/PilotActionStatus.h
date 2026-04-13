#pragma once

#include "Pilot/PilotTypes.h"

namespace Pilot {

enum class PilotActionStatus {
    PENDING,
    DELIVERED,
    ACKNOWLEDGED,
    UNKNOWN
};

const char* pilotActionStatusValue(PilotActionStatus s);
PilotActionStatus pilotActionStatusFromValue(const PilotString& v);

} // namespace Pilot
