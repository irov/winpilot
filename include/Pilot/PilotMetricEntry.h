#pragma once

#include "Pilot/PilotJson.h"
#include "Pilot/PilotMetricType.h"
#include <cstdint>
#include <chrono>

namespace Pilot {

class PilotMetricEntry {
public:
    PilotMetricEntry(const PilotMetricType& type, double value);
    PilotMetricEntry(const PilotMetricType& type, double value, int64_t timestampMs);

    const PilotMetricType& type() const;
    double value() const;
    int64_t timestampMs() const;

    PilotJson toJson() const;

private:
    PilotMetricType m_type;
    double m_value;
    int64_t m_timestampMs;
};

} // namespace Pilot
