#pragma once

#include "Pilot/PilotWidget.h"
#include "Pilot/PilotWidgetCallback.h"

namespace Pilot {

class PilotButton : public PilotWidget {
public:
    PilotButton(PilotUI& ui, const PilotString& label);

    PilotButton& variant(const PilotString& v);
    PilotButton& color(const PilotString& c);
    PilotButton& disabled(bool d);

    PilotButton& onClick(PilotWidgetCallback* callback);
    PilotButton& onClick(PilotWidgetCallbackFunc callback);
};

} // namespace Pilot
