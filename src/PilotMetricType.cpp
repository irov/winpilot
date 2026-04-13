#include "Pilot/PilotMetricType.h"

#include <cstring>

namespace Pilot {

PilotMetricType::PilotMetricType(const char* key, const char* unit, PilotMetricAggregation aggregation)
    : m_key(key), m_unit(unit), m_aggregation(aggregation) {
}

const char* PilotMetricType::key() const {
    return m_key;
}

const char* PilotMetricType::unit() const {
    return m_unit;
}

PilotMetricAggregation PilotMetricType::aggregation() const {
    return m_aggregation;
}

bool PilotMetricType::operator==(const PilotMetricType& o) const {
    return strcmp(m_key, o.m_key) == 0;
}

bool PilotMetricType::operator!=(const PilotMetricType& o) const {
    return strcmp(m_key, o.m_key) != 0;
}

PilotMetricType PilotMetricType::create(const char* key) {
    return PilotMetricType(key, "", PilotMetricAggregation::GAUGE);
}

PilotMetricType PilotMetricType::create(const char* key, const char* unit) {
    return PilotMetricType(key, unit, PilotMetricAggregation::GAUGE);
}

PilotMetricType PilotMetricType::create(const char* key, const char* unit, PilotMetricAggregation agg) {
    return PilotMetricType(key, unit, agg);
}

const PilotMetricType PilotMetricType::FPS{"fps", "", PilotMetricAggregation::GAUGE};
const PilotMetricType PilotMetricType::FRAME_TIME{"frame_time", "ms", PilotMetricAggregation::GAUGE};
const PilotMetricType PilotMetricType::MEMORY{"memory", "bytes", PilotMetricAggregation::GAUGE};
const PilotMetricType PilotMetricType::VIDEO_MEMORY{"video_memory", "bytes", PilotMetricAggregation::GAUGE};
const PilotMetricType PilotMetricType::CPU_USAGE{"cpu_usage", "%", PilotMetricAggregation::GAUGE};
const PilotMetricType PilotMetricType::NETWORK_RX{"network_rx", "bytes/s", PilotMetricAggregation::RATE};
const PilotMetricType PilotMetricType::NETWORK_TX{"network_tx", "bytes/s", PilotMetricAggregation::RATE};
const PilotMetricType PilotMetricType::DRAW_CALLS{"draw_calls", "", PilotMetricAggregation::GAUGE};
const PilotMetricType PilotMetricType::THREAD_COUNT{"thread_count", "", PilotMetricAggregation::GAUGE};

} // namespace Pilot
