#pragma once

#include <Pilot/PilotWidget.h>
#include <Pilot/PilotWidgetCallback.h>
#include <Pilot/PilotWidgetActions.h>

namespace Pilot {

class PilotSwitch : public PilotWidget {
public:
    PilotSwitch(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "switch") {
        put("label", PilotJson(label));
    }

    PilotSwitch& defaultValue(bool v) { put("defaultValue", PilotJson(v)); return *this; }

    PilotSwitch& onChange(PilotWidgetCallback* callback);
    PilotSwitch& onChange(PilotWidgetCallbackFunc callback);
    PilotSwitch& onChange(PilotFunction<void(const PilotSwitchAction&)> callback);
};

} // namespace Pilot
