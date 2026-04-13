#include <Pilot/PilotLogEntry.h>

#include <chrono>
#include <ctime>

namespace Pilot {

static PilotString formatTimestampImpl(int64_t ms) {
    time_t seconds = static_cast<time_t>(ms / 1000);
    struct tm utcBuf;
#ifdef _WIN32
    gmtime_s(&utcBuf, &seconds);
#else
    gmtime_r(&seconds, &utcBuf);
#endif
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utcBuf);
    return buf;
}

static int64_t currentTimeMsImpl() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

PilotLogEntry::PilotLogEntry(PilotLogLevel level, const PilotString& message,
                             const PilotString& category, const PilotString& thread,
                             const PilotMap<PilotString, PilotJson>& metadata,
                             const PilotJson& attributes, int64_t clientTimestampMs)
    : m_level(pilotLogLevelValue(level))
    , m_message(message)
    , m_category(category)
    , m_thread(thread)
    , m_metadata(metadata)
    , m_attributes(attributes)
    , m_clientTimestamp(formatTimestampImpl(clientTimestampMs > 0 ? clientTimestampMs : currentTimeMsImpl())) {
}

PilotLogEntry::PilotLogEntry(const PilotString& level, const PilotString& message,
                             const PilotString& category, const PilotString& thread,
                             const PilotMap<PilotString, PilotJson>& metadata,
                             const PilotJson& attributes, int64_t clientTimestampMs)
    : m_level(level)
    , m_message(message)
    , m_category(category)
    , m_thread(thread)
    , m_metadata(metadata)
    , m_attributes(attributes)
    , m_clientTimestamp(formatTimestampImpl(clientTimestampMs > 0 ? clientTimestampMs : currentTimeMsImpl())) {
}

PilotJson PilotLogEntry::toJson() const {
    PilotJson json = PilotJson::object();
    json.put("level", PilotJson(m_level));
    json.put("message", PilotJson(m_message));
    json.put("client_timestamp", PilotJson(m_clientTimestamp));
    if (!m_category.empty()) {
        json.put("category", PilotJson(m_category));
    }
    if (!m_thread.empty()) {
        json.put("thread", PilotJson(m_thread));
    }
    if (!m_metadata.empty()) {
        PilotJson metaJson = PilotJson::object();
        for (const auto& kv : m_metadata) {
            metaJson.put(kv.first, kv.second);
        }
        json.put("metadata", metaJson);
    }
    if (!m_attributes.isNull()) {
        json.put("attributes", m_attributes);
    }
    return json;
}

PilotString PilotLogEntry::formatTimestamp(int64_t ms) { return formatTimestampImpl(ms); }
int64_t PilotLogEntry::currentTimeMs() { return currentTimeMsImpl(); }

PilotLogEntry PilotLogEntry::debug(const PilotString& msg) { return PilotLogEntry(PilotLogLevel::DEBUG, msg); }
PilotLogEntry PilotLogEntry::info(const PilotString& msg) { return PilotLogEntry(PilotLogLevel::INFO, msg); }
PilotLogEntry PilotLogEntry::warning(const PilotString& msg) { return PilotLogEntry(PilotLogLevel::WARNING, msg); }
PilotLogEntry PilotLogEntry::error(const PilotString& msg) { return PilotLogEntry(PilotLogLevel::ERROR, msg); }
PilotLogEntry PilotLogEntry::critical(const PilotString& msg) { return PilotLogEntry(PilotLogLevel::CRITICAL, msg); }

} // namespace Pilot
