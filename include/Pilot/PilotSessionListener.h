#pragma once

#include "Pilot/PilotLogLevel.h"
#include "Pilot/PilotTypes.h"

namespace Pilot {

class PilotException;

class PilotSessionListener {
public:
    virtual ~PilotSessionListener() = default;
    virtual void onPilotSessionConnecting() {}
    virtual void onPilotSessionWaitingApproval(const PilotString& requestId) { (void)requestId; }
    virtual void onPilotSessionStarted(const PilotString& sessionToken) { (void)sessionToken; }
    virtual void onPilotSessionClosed() {}
    virtual void onPilotSessionRejected() {}
    virtual void onPilotSessionAuthFailed() {}
    virtual void onPilotSessionError(const PilotException& exception) { (void)exception; }
};

} // namespace Pilot
