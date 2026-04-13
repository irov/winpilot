#pragma once

#include <Pilot/PilotTypes.h>
#include <exception>

namespace Pilot {

class PilotException : public std::exception {
public:
    PilotException(const PilotString& message)
        : m_message(message), m_httpCode(0) {}

    PilotException(int httpCode, const PilotString& message)
        : m_message(message), m_httpCode(httpCode) {}

    const char* what() const noexcept override { return m_message.c_str(); }
    int httpCode() const { return m_httpCode; }

    bool isNetworkError() const { return m_httpCode == 0; }
    bool isSessionGone() const { return m_httpCode == 410; }
    bool isUnauthorized() const { return m_httpCode == 401; }

private:
    PilotString m_message;
    int m_httpCode;
};

} // namespace Pilot
