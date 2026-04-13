#pragma once

#include <Pilot/PilotTypes.h>

// Windows headers define ERROR as a macro — protect our enum
#pragma push_macro("ERROR")
#undef ERROR

namespace Pilot {

enum class PilotLogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL,
    EXCEPTION
};

inline const char* pilotLogLevelValue(PilotLogLevel level) {
    switch (level) {
        case PilotLogLevel::DEBUG: return "debug";
        case PilotLogLevel::INFO: return "info";
        case PilotLogLevel::WARNING: return "warning";
        case PilotLogLevel::ERROR: return "error";
        case PilotLogLevel::CRITICAL: return "critical";
        case PilotLogLevel::EXCEPTION: return "exception";
    }
    return "info";
}

} // namespace Pilot

#pragma pop_macro("ERROR")
