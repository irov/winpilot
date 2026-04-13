#pragma once

#include <Pilot/PilotJson.h>

namespace Pilot {

class PilotConnectResponse {
public:
    PilotConnectResponse(const PilotString& requestId, const PilotString& status, const PilotString& sessionToken)
        : m_requestId(requestId), m_status(status), m_sessionToken(sessionToken) {}

    const PilotString& requestId() const { return m_requestId; }
    const PilotString& status() const { return m_status; }
    const PilotString& sessionToken() const { return m_sessionToken; }

    bool isPending() const { return m_status == "pending"; }
    bool isApproved() const { return m_status == "approved"; }
    bool isRejected() const { return m_status == "rejected"; }

    static PilotConnectResponse fromJson(const PilotJson& json) {
        return PilotConnectResponse(
            json.optString("request_id"),
            json.optString("status"),
            json.optString("session_token")
        );
    }

private:
    PilotString m_requestId;
    PilotString m_status;
    PilotString m_sessionToken;
};

} // namespace Pilot
