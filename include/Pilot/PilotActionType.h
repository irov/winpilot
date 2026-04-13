#pragma once

#include <Pilot/PilotTypes.h>

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

inline const char* pilotActionTypeValue(PilotActionType t) {
    switch (t) {
        case PilotActionType::CLICK: return "click";
        case PilotActionType::CHANGE: return "change";
        case PilotActionType::TOGGLE: return "toggle";
        case PilotActionType::LIVE_START: return "live_start";
        case PilotActionType::LIVE_UPDATE: return "live_update";
        case PilotActionType::LIVE_STOP: return "live_stop";
        case PilotActionType::LIVE_TAP: return "live_tap";
        case PilotActionType::LIVE_LONG_PRESS: return "live_long_press";
        case PilotActionType::UNKNOWN: return "";
    }
    return "";
}

inline PilotActionType pilotActionTypeFromValue(const PilotString& v) {
    if (v == "click") return PilotActionType::CLICK;
    if (v == "change") return PilotActionType::CHANGE;
    if (v == "toggle") return PilotActionType::TOGGLE;
    if (v == "live_start") return PilotActionType::LIVE_START;
    if (v == "live_update") return PilotActionType::LIVE_UPDATE;
    if (v == "live_stop") return PilotActionType::LIVE_STOP;
    if (v == "live_tap") return PilotActionType::LIVE_TAP;
    if (v == "live_long_press") return PilotActionType::LIVE_LONG_PRESS;
    return PilotActionType::UNKNOWN;
}

} // namespace Pilot
