#pragma once

namespace Pilot {

enum class PilotSessionStatus {
    PL_DISCONNECTED,
    PL_CONNECTING,
    PL_WAITING_APPROVAL,
    PL_ACTIVE,
    PL_AUTH_FAILED,
    PL_REJECTED,
    PL_CLOSED,
    PL_ERROR
};

} // namespace Pilot
