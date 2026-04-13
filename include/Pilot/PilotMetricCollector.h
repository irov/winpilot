#pragma once

#include "Pilot/PilotTypes.h"

namespace Pilot {

class PilotMetricEntry;

class PilotMetricCollector {
public:
    virtual ~PilotMetricCollector() = default;
    virtual void collect(PilotVector<PilotMetricEntry>& out) = 0;
};

} // namespace Pilot
