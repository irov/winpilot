#include "Pilot/PilotAction.h"

namespace Pilot {

PilotAction::PilotAction()
    : m_widgetId(0), m_actionType(PilotActionType::UNKNOWN), m_status(PilotActionStatus::UNKNOWN) {
}

PilotAction::PilotAction(const PilotString& id, const PilotString& sessionId, int widgetId,
                         PilotActionType actionType, PilotActionStatus status, const PilotJson& payload)
    : m_id(id), m_sessionId(sessionId), m_widgetId(widgetId)
    , m_actionType(actionType), m_status(status), m_payload(payload) {
}

const PilotString& PilotAction::id() const { return m_id; }
const PilotString& PilotAction::sessionId() const { return m_sessionId; }
int PilotAction::widgetId() const { return m_widgetId; }
PilotActionType PilotAction::actionType() const { return m_actionType; }
PilotActionStatus PilotAction::actionStatus() const { return m_status; }
const PilotJson& PilotAction::payload() const { return m_payload; }

PilotAction PilotAction::fromJson(const PilotJson& json) {
    return PilotAction(
        json.optString("id"),
        json.optString("session_id"),
        json.optInt("widget_id", 0),
        pilotActionTypeFromValue(json.optString("action_type")),
        pilotActionStatusFromValue(json.optString("status")),
        json.optObject("payload")
    );
}

} // namespace Pilot
