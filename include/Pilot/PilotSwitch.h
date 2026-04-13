#pragma once

#include "Pilot/PilotWidget.h"
#include "Pilot/PilotWidgetCallback.h"
#include "Pilot/PilotWidgetActions.h"

namespace Pilot {

class PilotSwitch : public PilotWidget {
public:
    PilotSwitch(PilotUI& ui, const PilotString& label);

    PilotSwitch& defaultValue(bool v);

    PilotSwitch& onChange(PilotWidgetCallback* callback);
    PilotSwitch& onChange(PilotWidgetCallbackFunc callback);
    PilotSwitch& onChange(PilotFunction<void(const PilotSwitchAction&)> callback);
};

} // namespace Pilot
