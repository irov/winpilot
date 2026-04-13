#include <Pilot/PilotButton.h>
#include <Pilot/PilotSwitch.h>
#include <Pilot/PilotInput.h>
#include <Pilot/PilotSelect.h>
#include <Pilot/PilotTextarea.h>
#include <Pilot/PilotUI.h>

namespace Pilot {

// ── PilotButton ──

PilotButton& PilotButton::onClick(PilotWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

PilotButton& PilotButton::onClick(PilotWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

// ── PilotSwitch ──

PilotSwitch& PilotSwitch::onChange(PilotWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

PilotSwitch& PilotSwitch::onChange(PilotWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

PilotSwitch& PilotSwitch::onChange(PilotFunction<void(const PilotSwitchAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const PilotAction& action) {
        cb(PilotSwitchAction(action));
    });
    return *this;
}

// ── PilotInput ──

PilotInput& PilotInput::onSubmit(PilotWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

PilotInput& PilotInput::onSubmit(PilotWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

PilotInput& PilotInput::onSubmit(PilotFunction<void(const PilotInputAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const PilotAction& action) {
        cb(PilotInputAction(action));
    });
    return *this;
}

// ── PilotSelect ──

PilotSelect& PilotSelect::onChange(PilotWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

PilotSelect& PilotSelect::onChange(PilotWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

PilotSelect& PilotSelect::onChange(PilotFunction<void(const PilotSelectAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const PilotAction& action) {
        cb(PilotSelectAction(action));
    });
    return *this;
}

// ── PilotTextarea ──

PilotTextarea& PilotTextarea::onSubmit(PilotWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

PilotTextarea& PilotTextarea::onSubmit(PilotWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

PilotTextarea& PilotTextarea::onSubmit(PilotFunction<void(const PilotTextareaAction&)> callback) {
    m_ui.registerCallbackFunc(internalId(), [cb = std::move(callback)](const PilotAction& action) {
        cb(PilotTextareaAction(action));
    });
    return *this;
}

} // namespace Pilot
