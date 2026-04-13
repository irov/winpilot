#pragma once

#include <Pilot/PilotWidget.h>
#include <Pilot/PilotWidgetCallback.h>
#include <Pilot/PilotWidgetActions.h>

namespace Pilot {

class PilotInput : public PilotWidget {
public:
    PilotInput(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "input") {
        put("label", PilotJson(label));
    }

    PilotInput& inputType(const PilotString& t) { put("inputType", PilotJson(t)); return *this; }
    PilotInput& defaultValue(const PilotString& v) { put("defaultValue", PilotJson(v)); return *this; }
    PilotInput& placeholder(const PilotString& p) { put("placeholder", PilotJson(p)); return *this; }

    PilotInput& onSubmit(PilotWidgetCallback* callback);
    PilotInput& onSubmit(PilotWidgetCallbackFunc callback);
    PilotInput& onSubmit(PilotFunction<void(const PilotInputAction&)> callback);
};

} // namespace Pilot
