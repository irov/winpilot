#pragma once

#include <Pilot/PilotWidget.h>
#include <Pilot/PilotWidgetCallback.h>
#include <Pilot/PilotWidgetActions.h>

namespace Pilot {

class PilotTextarea : public PilotWidget {
public:
    PilotTextarea(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "textarea") {
        put("label", PilotJson(label));
    }

    PilotTextarea& rows(int r) { put("rows", PilotJson(r)); return *this; }
    PilotTextarea& defaultValue(const PilotString& v) { put("defaultValue", PilotJson(v)); return *this; }

    PilotTextarea& onSubmit(PilotWidgetCallback* callback);
    PilotTextarea& onSubmit(PilotWidgetCallbackFunc callback);
    PilotTextarea& onSubmit(PilotFunction<void(const PilotTextareaAction&)> callback);
};

} // namespace Pilot
