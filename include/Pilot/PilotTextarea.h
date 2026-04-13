#pragma once

#include "Pilot/PilotWidget.h"
#include "Pilot/PilotWidgetCallback.h"
#include "Pilot/PilotWidgetActions.h"

namespace Pilot {

class PilotTextarea : public PilotWidget {
public:
    PilotTextarea(PilotUI& ui, const PilotString& label);

    PilotTextarea& rows(int r);
    PilotTextarea& defaultValue(const PilotString& v);

    PilotTextarea& onSubmit(PilotWidgetCallback* callback);
    PilotTextarea& onSubmit(PilotWidgetCallbackFunc callback);
    PilotTextarea& onSubmit(PilotFunction<void(const PilotTextareaAction&)> callback);
};

} // namespace Pilot
