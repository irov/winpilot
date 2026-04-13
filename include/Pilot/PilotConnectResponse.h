#pragma once

#include "Pilot/PilotJson.h"

namespace Pilot {

class PilotConnectResponse {
public:
    PilotConnectResponse(const PilotString& requestId, const PilotString& status, const PilotString& sessionToken);

    const PilotString& requestId() const;
    const PilotString& status() const;
    const PilotString& sessionToken() const;

    bool isPending() const;
    bool isApproved() const;
    bool isRejected() const;

    static PilotConnectResponse fromJson(const PilotJson& json);

private:
    PilotString m_requestId;
    PilotString m_status;
    PilotString m_sessionToken;
};

} // namespace Pilot
