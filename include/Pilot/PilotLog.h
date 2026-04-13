#pragma once

#include <Pilot/PilotLogLevel.h>
#include <Pilot/PilotLoggerListener.h>
#include <Pilot/PilotTypes.h>

namespace Pilot {

class PilotLog {
public:
    static void setLevel(PilotLogLevel level);
    static void setLoggerListener(PilotLoggerListener* listener);

    static void d(const char* fmt, ...);
    static void i(const char* fmt, ...);
    static void w(const char* fmt, ...);
    static void e(const char* fmt, ...);

private:
    static PilotLogLevel s_level;
    static PilotLoggerListener* s_listener;
    static const char* TAG;

    static void logMessage(PilotLogLevel level, const char* fmt, va_list args);
};

} // namespace Pilot
