#pragma once

#include <Pilot/PilotTypes.h>

namespace Pilot {

class PilotAction;

class PilotWidgetCallback {
public:
    virtual ~PilotWidgetCallback() = default;
    virtual void onPilotWidgetAction(const PilotAction& action) = 0;
};

using PilotWidgetCallbackFunc = PilotFunction<void(const PilotAction&)>;

} // namespace Pilot
