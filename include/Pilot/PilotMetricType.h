#pragma once

#include "Pilot/PilotMetricAggregation.h"
#include "Pilot/PilotTypes.h"

namespace Pilot {

class PilotMetricType {
public:
    PilotMetricType(const PilotString& key, const PilotString& unit, PilotMetricAggregation aggregation)
        : m_key(key), m_unit(unit), m_aggregation(aggregation) {}

    const PilotString& key() const { return m_key; }
    const PilotString& unit() const { return m_unit; }
    PilotMetricAggregation aggregation() const { return m_aggregation; }

    bool operator==(const PilotMetricType& o) const { return m_key == o.m_key; }
    bool operator!=(const PilotMetricType& o) const { return m_key != o.m_key; }

    static PilotMetricType create(const PilotString& key) {
        return PilotMetricType(key, "", PilotMetricAggregation::GAUGE);
    }
    static PilotMetricType create(const PilotString& key, const PilotString& unit) {
        return PilotMetricType(key, unit, PilotMetricAggregation::GAUGE);
    }
    static PilotMetricType create(const PilotString& key, const PilotString& unit, PilotMetricAggregation agg) {
        return PilotMetricType(key, unit, agg);
    }

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
    PilotString m_key;
    PilotString m_unit;
    PilotMetricAggregation m_aggregation;
};

inline const PilotMetricType PilotMetricType::FPS{"fps", "", PilotMetricAggregation::GAUGE};
inline const PilotMetricType PilotMetricType::FRAME_TIME{"frame_time", "ms", PilotMetricAggregation::GAUGE};
inline const PilotMetricType PilotMetricType::MEMORY{"memory", "bytes", PilotMetricAggregation::GAUGE};
inline const PilotMetricType PilotMetricType::VIDEO_MEMORY{"video_memory", "bytes", PilotMetricAggregation::GAUGE};
inline const PilotMetricType PilotMetricType::CPU_USAGE{"cpu_usage", "%", PilotMetricAggregation::GAUGE};
inline const PilotMetricType PilotMetricType::NETWORK_RX{"network_rx", "bytes/s", PilotMetricAggregation::RATE};
inline const PilotMetricType PilotMetricType::NETWORK_TX{"network_tx", "bytes/s", PilotMetricAggregation::RATE};
inline const PilotMetricType PilotMetricType::DRAW_CALLS{"draw_calls", "", PilotMetricAggregation::GAUGE};
inline const PilotMetricType PilotMetricType::THREAD_COUNT{"thread_count", "", PilotMetricAggregation::GAUGE};

} // namespace Pilot
