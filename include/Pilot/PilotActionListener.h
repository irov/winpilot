#pragma once

namespace Pilot {

class PilotAction;

class PilotActionListener {
public:
    virtual ~PilotActionListener() = default;
    virtual void onPilotActionReceived(const PilotAction& action) = 0;
};

} // namespace Pilot
