#pragma once

#include "Pilot/PilotAction.h"

namespace Pilot {

class PilotSwitchAction {
public:
    explicit PilotSwitchAction(const PilotAction& action);
    bool value() const;
    const PilotAction& action() const;
private:
    PilotAction m_action;
};

class PilotInputAction {
public:
    explicit PilotInputAction(const PilotAction& action);
    PilotString value() const;
    const PilotAction& action() const;
private:
    PilotAction m_action;
};

class PilotSelectAction {
public:
    explicit PilotSelectAction(const PilotAction& action);
    PilotString value() const;
    const PilotAction& action() const;
private:
    PilotAction m_action;
};

class PilotTextareaAction {
public:
    explicit PilotTextareaAction(const PilotAction& action);
    PilotString value() const;
    const PilotAction& action() const;
private:
    PilotAction m_action;
};

} // namespace Pilot
