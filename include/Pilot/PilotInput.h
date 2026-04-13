#pragma once

#include "Pilot/PilotWidget.h"
#include "Pilot/PilotWidgetCallback.h"
#include "Pilot/PilotWidgetActions.h"

namespace Pilot {

class PilotInput : public PilotWidget {
public:
    PilotInput(PilotUI& ui, const PilotString& label);

    PilotInput& inputType(const PilotString& t);
    PilotInput& defaultValue(const PilotString& v);
    PilotInput& placeholder(const PilotString& p);

    PilotInput& onSubmit(PilotWidgetCallback* callback);
    PilotInput& onSubmit(PilotWidgetCallbackFunc callback);
    PilotInput& onSubmit(PilotFunction<void(const PilotInputAction&)> callback);
};

} // namespace Pilot
