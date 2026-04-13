#pragma once

#include <Pilot/PilotLogLevel.h>
#include <Pilot/PilotTypes.h>

namespace Pilot {

class PilotLoggerListener {
public:
    virtual ~PilotLoggerListener() = default;
    virtual void onPilotLoggerMessage(PilotLogLevel level, const PilotString& tag, const PilotString& message) = 0;
};

} // namespace Pilot
