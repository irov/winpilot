#pragma once

#include "Pilot/PilotWidget.h"

namespace Pilot {

class PilotLabel : public PilotWidget {
public:
    PilotLabel(PilotUI& ui, const PilotString& text);

    PilotLabel& text(const PilotString& t);
    PilotLabel& color(const PilotString& c);

    PilotLabel& textProvider(PilotValueProvider* provider);
};

} // namespace Pilot
