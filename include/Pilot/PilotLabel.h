#pragma once

#include <Pilot/PilotWidget.h>

namespace Pilot {

class PilotLabel : public PilotWidget {
public:
    PilotLabel(PilotUI& ui, const PilotString& text)
        : PilotWidget(ui, "label") {
        put("text", PilotJson(text));
    }

    PilotLabel& text(const PilotString& t) { put("text", PilotJson(t)); return *this; }
    PilotLabel& color(const PilotString& c) { put("color", PilotJson(c)); return *this; }

    PilotLabel& textProvider(PilotValueProvider* provider) {
        setProvider("text", provider);
        return *this;
    }
};

} // namespace Pilot
