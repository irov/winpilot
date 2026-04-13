#pragma once

#include "Pilot/PilotTypes.h"

namespace Pilot {

enum class PilotActionType {
    CLICK,
    CHANGE,
    TOGGLE,
    LIVE_START,
    LIVE_UPDATE,
    LIVE_STOP,
    LIVE_TAP,
    LIVE_LONG_PRESS,
    UNKNOWN
};

const char* pilotActionTypeValue(PilotActionType t);
PilotActionType pilotActionTypeFromValue(const PilotString& v);

} // namespace Pilot
