#pragma once

namespace Pilot {

enum class PilotLogLevel {
    PL_DEBUG,
    PL_INFO,
    PL_WARNING,
    PL_ERROR,
    PL_CRITICAL,
    PL_EXCEPTION
};

const char* pilotLogLevelValue(PilotLogLevel level);

} // namespace Pilot
