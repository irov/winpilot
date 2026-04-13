#pragma once

#include "Pilot/PilotWidget.h"
#include "Pilot/PilotWidgetCallback.h"
#include "Pilot/PilotWidgetActions.h"

namespace Pilot {

class PilotSelect : public PilotWidget {
public:
    PilotSelect(PilotUI& ui, const PilotString& label);

    PilotSelect& options(const PilotVector<PilotPair<PilotString, PilotString>>& opts);
    PilotSelect& defaultValue(const PilotString& v);

    PilotSelect& onChange(PilotWidgetCallback* callback);
    PilotSelect& onChange(PilotWidgetCallbackFunc callback);
    PilotSelect& onChange(PilotFunction<void(const PilotSelectAction&)> callback);
};

} // namespace Pilot
