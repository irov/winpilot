#include "Pilot/PilotWidgetActions.h"

namespace Pilot {

// ── PilotSwitchAction ──

PilotSwitchAction::PilotSwitchAction(const PilotAction& action) : m_action(action) {}
bool PilotSwitchAction::value() const { return m_action.payload().optBool("value", false); }
const PilotAction& PilotSwitchAction::action() const { return m_action; }

// ── PilotInputAction ──

PilotInputAction::PilotInputAction(const PilotAction& action) : m_action(action) {}
PilotString PilotInputAction::value() const { return m_action.payload().optString("value"); }
const PilotAction& PilotInputAction::action() const { return m_action; }

// ── PilotSelectAction ──

PilotSelectAction::PilotSelectAction(const PilotAction& action) : m_action(action) {}
PilotString PilotSelectAction::value() const { return m_action.payload().optString("value"); }
const PilotAction& PilotSelectAction::action() const { return m_action; }

// ── PilotTextareaAction ──

PilotTextareaAction::PilotTextareaAction(const PilotAction& action) : m_action(action) {}
PilotString PilotTextareaAction::value() const { return m_action.payload().optString("value"); }
const PilotAction& PilotTextareaAction::action() const { return m_action; }

} // namespace Pilot
