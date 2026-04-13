#pragma once

#include <Pilot/PilotAction.h>

namespace Pilot {

class PilotSwitchAction {
public:
    explicit PilotSwitchAction(const PilotAction& action) : m_action(action) {}
    bool value() const { return m_action.payload().optBool("value", false); }
    const PilotAction& action() const { return m_action; }
private:
    PilotAction m_action;
};

class PilotInputAction {
public:
    explicit PilotInputAction(const PilotAction& action) : m_action(action) {}
    PilotString value() const { return m_action.payload().optString("value"); }
    const PilotAction& action() const { return m_action; }
private:
    PilotAction m_action;
};

class PilotSelectAction {
public:
    explicit PilotSelectAction(const PilotAction& action) : m_action(action) {}
    PilotString value() const { return m_action.payload().optString("value"); }
    const PilotAction& action() const { return m_action; }
private:
    PilotAction m_action;
};

class PilotTextareaAction {
public:
    explicit PilotTextareaAction(const PilotAction& action) : m_action(action) {}
    PilotString value() const { return m_action.payload().optString("value"); }
    const PilotAction& action() const { return m_action; }
private:
    PilotAction m_action;
};

} // namespace Pilot
