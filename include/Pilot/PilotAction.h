#pragma once

#include <Pilot/PilotJson.h>
#include <Pilot/PilotActionType.h>
#include <Pilot/PilotActionStatus.h>

namespace Pilot {

class PilotAction {
public:
    PilotAction() : m_widgetId(0), m_actionType(PilotActionType::UNKNOWN), m_status(PilotActionStatus::UNKNOWN) {}

    PilotAction(const PilotString& id, const PilotString& sessionId, int widgetId,
                PilotActionType actionType, PilotActionStatus status, const PilotJson& payload)
        : m_id(id), m_sessionId(sessionId), m_widgetId(widgetId)
        , m_actionType(actionType), m_status(status), m_payload(payload) {}

    const PilotString& id() const { return m_id; }
    const PilotString& sessionId() const { return m_sessionId; }
    int widgetId() const { return m_widgetId; }
    PilotActionType actionType() const { return m_actionType; }
    PilotActionStatus actionStatus() const { return m_status; }
    const PilotJson& payload() const { return m_payload; }

    static PilotAction fromJson(const PilotJson& json) {
        return PilotAction(
            json.optString("id"),
            json.optString("session_id"),
            json.optInt("widget_id", 0),
            pilotActionTypeFromValue(json.optString("action_type")),
            pilotActionStatusFromValue(json.optString("status")),
            json.optObject("payload")
        );
    }

private:
    PilotString m_id;
    PilotString m_sessionId;
    int m_widgetId;
    PilotActionType m_actionType;
    PilotActionStatus m_status;
    PilotJson m_payload;
};

} // namespace Pilot
