#pragma once

#include <Pilot/PilotWidget.h>

namespace Pilot {

class PilotStat : public PilotWidget {
public:
    PilotStat(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "stat") {
        put("label", PilotJson(label));
    }

    PilotStat& value(const PilotString& v) { put("value", PilotJson(v)); return *this; }
    PilotStat& unit(const PilotString& u) { put("unit", PilotJson(u)); return *this; }

    PilotStat& valueProvider(PilotValueProvider* provider) {
        setProvider("value", provider);
        return *this;
    }
};

} // namespace Pilot
