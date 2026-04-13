#pragma once

#include "Pilot/PilotTypes.h"

namespace Pilot {

class PilotValueProvider {
public:
    virtual ~PilotValueProvider() = default;
    virtual PilotString getValue() = 0;
};

} // namespace Pilot
