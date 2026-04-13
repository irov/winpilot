#pragma once

#include "Pilot/PilotTypes.h"
#include <exception>

namespace Pilot {

class PilotException : public std::exception {
public:
    PilotException(const PilotString& message);
    PilotException(int httpCode, const PilotString& message);

    const char* what() const noexcept override;
    int httpCode() const;

    bool isNetworkError() const;
    bool isSessionGone() const;
    bool isUnauthorized() const;

private:
    PilotString m_message;
    int m_httpCode;
};

} // namespace Pilot
