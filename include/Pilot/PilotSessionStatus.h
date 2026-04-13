#pragma once

// Windows headers define ERROR as a macro — protect our enum
#pragma push_macro("ERROR")
#undef ERROR

namespace Pilot {

enum class PilotSessionStatus {
    DISCONNECTED,
    CONNECTING,
    WAITING_APPROVAL,
    ACTIVE,
    AUTH_FAILED,
    REJECTED,
    CLOSED,
    ERROR
};

} // namespace Pilot

#pragma pop_macro("ERROR")
