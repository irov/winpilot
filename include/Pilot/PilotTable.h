#pragma once

#include "Pilot/PilotWidget.h"

namespace Pilot {

class PilotTable : public PilotWidget {
public:
    PilotTable(PilotUI& ui, const PilotString& label);

    PilotTable& columns(const PilotVector<PilotPair<PilotString, PilotString>>& cols);
    PilotTable& rows(const PilotVector<PilotJson>& rowData);
};

} // namespace Pilot
