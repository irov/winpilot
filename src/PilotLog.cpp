#include <Pilot/PilotLog.h>

#include <cstdio>
#include <cstdarg>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
// Restore ERROR after windows.h might undefine it
#pragma push_macro("ERROR")
#undef ERROR
#endif

namespace Pilot {

PilotLogLevel PilotLog::s_level = PilotLogLevel::INFO;
PilotLoggerListener* PilotLog::s_listener = nullptr;
const char* PilotLog::TAG = "PilotSDK";

void PilotLog::setLevel(PilotLogLevel level) {
    s_level = level;
}

void PilotLog::setLoggerListener(PilotLoggerListener* listener) {
    s_listener = listener;
}

void PilotLog::logMessage(PilotLogLevel level, const char* fmt, va_list args) {
    if (static_cast<int>(level) < static_cast<int>(s_level)) {
        return;
    }

    char buf[1024];
    vsnprintf(buf, sizeof(buf), fmt, args);

    if (s_listener) {
        s_listener->onPilotLoggerMessage(level, TAG, buf);
        return;
    }

    const char* levelStr = pilotLogLevelValue(level);

#ifdef _WIN32
    char dbgBuf[1100];
    snprintf(dbgBuf, sizeof(dbgBuf), "[%s] %s: %s\n", levelStr, TAG, buf);
    OutputDebugStringA(dbgBuf);
#endif

    fprintf(stderr, "[%s] %s: %s\n", levelStr, TAG, buf);
}

void PilotLog::d(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(PilotLogLevel::DEBUG, fmt, args);
    va_end(args);
}

void PilotLog::i(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(PilotLogLevel::INFO, fmt, args);
    va_end(args);
}

void PilotLog::w(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(PilotLogLevel::WARNING, fmt, args);
    va_end(args);
}

void PilotLog::e(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    logMessage(PilotLogLevel::ERROR, fmt, args);
    va_end(args);
}

} // namespace Pilot

#ifdef _WIN32
#pragma pop_macro("ERROR")
#endif
