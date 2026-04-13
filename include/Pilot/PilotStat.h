#pragma once

#include "Pilot/PilotWidget.h"

namespace Pilot {

class PilotStat : public PilotWidget {
public:
    PilotStat(PilotUI& ui, const PilotString& label);

    PilotStat& value(const PilotString& v);
    PilotStat& unit(const PilotString& u);

    PilotStat& valueProvider(PilotValueProvider* provider);
};

} // namespace Pilot
