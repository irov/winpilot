#include "Pilot/PilotException.h"

namespace Pilot {

PilotException::PilotException(const PilotString& message)
    : m_message(message), m_httpCode(0) {
}

PilotException::PilotException(int httpCode, const PilotString& message)
    : m_message(message), m_httpCode(httpCode) {
}

const char* PilotException::what() const noexcept { return m_message.c_str(); }
int PilotException::httpCode() const { return m_httpCode; }

bool PilotException::isNetworkError() const { return m_httpCode == 0; }
bool PilotException::isSessionGone() const { return m_httpCode == 410; }
bool PilotException::isUnauthorized() const { return m_httpCode == 401; }

} // namespace Pilot
