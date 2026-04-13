#pragma once

#include <Pilot/PilotWidget.h>

namespace Pilot {

class PilotLogs : public PilotWidget {
public:
    PilotLogs(PilotUI& ui, const PilotString& label)
        : PilotWidget(ui, "logs") {
        put("label", PilotJson(label));
    }

    PilotLogs& maxLines(int v) { put("maxLines", PilotJson(v)); return *this; }
};

} // namespace Pilot
