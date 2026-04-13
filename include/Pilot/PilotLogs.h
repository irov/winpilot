#pragma once

#include "Pilot/PilotWidget.h"

namespace Pilot {

class PilotLogs : public PilotWidget {
public:
    PilotLogs(PilotUI& ui, const PilotString& label);

    PilotLogs& maxLines(int v);
};

} // namespace Pilot
