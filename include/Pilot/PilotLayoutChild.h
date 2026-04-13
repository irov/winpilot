#pragma once

#include "Pilot/PilotJson.h"

namespace Pilot {

class PilotLayoutChild {
public:
    virtual ~PilotLayoutChild() = default;
    virtual PilotJson toJson() const = 0;
};

} // namespace Pilot
