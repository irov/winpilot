#include "Pilot/PilotConnectResponse.h"

namespace Pilot {

PilotConnectResponse::PilotConnectResponse(const PilotString& requestId, const PilotString& status, const PilotString& sessionToken)
    : m_requestId(requestId), m_status(status), m_sessionToken(sessionToken) {
}

const PilotString& PilotConnectResponse::requestId() const { return m_requestId; }
const PilotString& PilotConnectResponse::status() const { return m_status; }
const PilotString& PilotConnectResponse::sessionToken() const { return m_sessionToken; }

bool PilotConnectResponse::isPending() const { return m_status == "pending"; }
bool PilotConnectResponse::isApproved() const { return m_status == "approved"; }
bool PilotConnectResponse::isRejected() const { return m_status == "rejected"; }

PilotConnectResponse PilotConnectResponse::fromJson(const PilotJson& json) {
    return PilotConnectResponse(
        json.optString("request_id"),
        json.optString("status"),
        json.optString("session_token")
    );
}

} // namespace Pilot
