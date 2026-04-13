#pragma once

#include "Pilot/PilotJson.h"
#include "Pilot/PilotActionType.h"
#include "Pilot/PilotActionStatus.h"

namespace Pilot {

class PilotAction {
public:
    PilotAction();
    PilotAction(const PilotString& id, const PilotString& sessionId, int widgetId,
                PilotActionType actionType, PilotActionStatus status, const PilotJson& payload);

    const PilotString& id() const;
    const PilotString& sessionId() const;
    int widgetId() const;
    PilotActionType actionType() const;
    PilotActionStatus actionStatus() const;
    const PilotJson& payload() const;

    static PilotAction fromJson(const PilotJson& json);

private:
    PilotString m_id;
    PilotString m_sessionId;
    int m_widgetId;
    PilotActionType m_actionType;
    PilotActionStatus m_status;
    PilotJson m_payload;
};

} // namespace Pilot
