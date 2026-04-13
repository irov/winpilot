#include "Pilot/PilotLogLevel.h"

namespace Pilot {

const char* pilotLogLevelValue(PilotLogLevel level) {
    switch (level) {
        case PilotLogLevel::PL_DEBUG: return "debug";
        case PilotLogLevel::PL_INFO: return "info";
        case PilotLogLevel::PL_WARNING: return "warning";
        case PilotLogLevel::PL_ERROR: return "error";
        case PilotLogLevel::PL_CRITICAL: return "critical";
        case PilotLogLevel::PL_EXCEPTION: return "exception";
    }
    return "info";
}

} // namespace Pilot
