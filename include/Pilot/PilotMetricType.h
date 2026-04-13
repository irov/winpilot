#pragma once

#include "Pilot/PilotMetricAggregation.h"

namespace Pilot {

class PilotMetricType {
public:
    PilotMetricType(const char* key, const char* unit, PilotMetricAggregation aggregation);

    const char* key() const;
    const char* unit() const;
    PilotMetricAggregation aggregation() const;

    bool operator==(const PilotMetricType& o) const;
    bool operator!=(const PilotMetricType& o) const;

    static PilotMetricType create(const char* key);
    static PilotMetricType create(const char* key, const char* unit);
    static PilotMetricType create(const char* key, const char* unit, PilotMetricAggregation agg);

    static const PilotMetricType FPS;
    static const PilotMetricType FRAME_TIME;
    static const PilotMetricType MEMORY;
    static const PilotMetricType VIDEO_MEMORY;
    static const PilotMetricType CPU_USAGE;
    static const PilotMetricType NETWORK_RX;
    static const PilotMetricType NETWORK_TX;
    static const PilotMetricType DRAW_CALLS;
    static const PilotMetricType THREAD_COUNT;

private:
    const char* m_key;
    const char* m_unit;
    PilotMetricAggregation m_aggregation;
};

} // namespace Pilot
