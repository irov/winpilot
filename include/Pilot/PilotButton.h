#pragma once

#include <Pilot/PilotWidget.h>
#include <Pilot/PilotWidgetCallback.h>

namespace Pilot {

class PilotButton : public PilotWidget {
public:
    PilotButton(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "button") {
        put("label", PilotJson(label));
    }

    PilotButton& variant(const PilotString& v) { put("variant", PilotJson(v)); return *this; }
    PilotButton& color(const PilotString& c) { put("color", PilotJson(c)); return *this; }
    PilotButton& disabled(bool d) { put("disabled", PilotJson(d)); return *this; }

    PilotButton& onClick(PilotWidgetCallback* callback);
    PilotButton& onClick(PilotWidgetCallbackFunc callback);
};

} // namespace Pilot
