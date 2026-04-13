#pragma once

#include "Pilot/PilotJson.h"
#include "Pilot/PilotLogLevel.h"
#include <cstdint>

namespace Pilot {

class PilotLogEntry {
public:
    PilotLogEntry(PilotLogLevel level, const PilotString& message,
                  const PilotString& category = PilotString(),
                  const PilotString& thread = PilotString(),
                  const PilotMap<PilotString, PilotJson>& metadata = {},
                  const PilotJson& attributes = PilotJson(),
                  int64_t clientTimestampMs = 0);

    PilotLogEntry(const PilotString& level, const PilotString& message,
                  const PilotString& category = PilotString(),
                  const PilotString& thread = PilotString(),
                  const PilotMap<PilotString, PilotJson>& metadata = {},
                  const PilotJson& attributes = PilotJson(),
                  int64_t clientTimestampMs = 0);

    PilotJson toJson() const;

    static PilotLogEntry debug(const PilotString& msg);
    static PilotLogEntry info(const PilotString& msg);
    static PilotLogEntry warning(const PilotString& msg);
    static PilotLogEntry error(const PilotString& msg);
    static PilotLogEntry critical(const PilotString& msg);

private:
    PilotString m_level;
    PilotString m_message;
    PilotString m_category;
    PilotString m_thread;
    PilotMap<PilotString, PilotJson> m_metadata;
    PilotJson m_attributes;
    PilotString m_clientTimestamp;

    static PilotString formatTimestamp(int64_t ms);
    static int64_t currentTimeMs();
};

} // namespace Pilot
