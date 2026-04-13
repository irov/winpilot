#include "Pilot/PilotButton.h"
#include "Pilot/PilotSwitch.h"
#include "Pilot/PilotInput.h"
#include "Pilot/PilotSelect.h"
#include "Pilot/PilotTextarea.h"
#include "Pilot/PilotLabel.h"
#include "Pilot/PilotStat.h"
#include "Pilot/PilotTable.h"
#include "Pilot/PilotLogs.h"
#include "Pilot/PilotUI.h"

namespace Pilot {

// ── PilotButton ──

PilotButton::PilotButton(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "button") {
    put("label", PilotJson(label));
}

PilotButton& PilotButton::variant(const PilotString& v) { put("variant", PilotJson(v)); return *this; }
PilotButton& PilotButton::color(const PilotString& c) { put("color", PilotJson(c)); return *this; }
PilotButton& PilotButton::disabled(bool d) { put("disabled", PilotJson(d)); return *this; }

PilotButton& PilotButton::onClick(PilotWidgetCallback* callback) {
    m_ui.registerCallback(internalId(), callback);
    return *this;
}

PilotButton& PilotButton::onClick(PilotWidgetCallbackFunc callback) {
    m_ui.registerCallbackFunc(internalId(), std::move(callback));
    return *this;
}

// ── PilotSwitch ──

PilotSwitch::PilotSwitch(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "switch") {
    put("label", PilotJson(label));
}

PilotSwitch& PilotSwitch::defaultValue(bool v) { put("defaultValue", PilotJson(v)); return *this; }

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

PilotInput::PilotInput(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "input") {
    put("label", PilotJson(label));
}

PilotInput& PilotInput::inputType(const PilotString& t) { put("inputType", PilotJson(t)); return *this; }
PilotInput& PilotInput::defaultValue(const PilotString& v) { put("defaultValue", PilotJson(v)); return *this; }
PilotInput& PilotInput::placeholder(const PilotString& p) { put("placeholder", PilotJson(p)); return *this; }

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

PilotSelect::PilotSelect(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "select") {
    put("label", PilotJson(label));
}

PilotSelect& PilotSelect::options(const PilotVector<PilotPair<PilotString, PilotString>>& opts) {
    PilotJson arr = PilotJson::array();
    for (const auto& opt : opts) {
        arr.add(PilotJson::object()
            .put("value", PilotJson(opt.first))
            .put("label", PilotJson(opt.second)));
    }
    put("options", arr);
    return *this;
}

PilotSelect& PilotSelect::defaultValue(const PilotString& v) { put("defaultValue", PilotJson(v)); return *this; }

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

PilotTextarea::PilotTextarea(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "textarea") {
    put("label", PilotJson(label));
}

PilotTextarea& PilotTextarea::rows(int r) { put("rows", PilotJson(r)); return *this; }
PilotTextarea& PilotTextarea::defaultValue(const PilotString& v) { put("defaultValue", PilotJson(v)); return *this; }

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

// ── PilotLabel ──

PilotLabel::PilotLabel(PilotUI& ui, const PilotString& text)
    : PilotWidget(ui, "label") {
    put("text", PilotJson(text));
}

PilotLabel& PilotLabel::text(const PilotString& t) { put("text", PilotJson(t)); return *this; }
PilotLabel& PilotLabel::color(const PilotString& c) { put("color", PilotJson(c)); return *this; }

PilotLabel& PilotLabel::textProvider(PilotValueProvider* provider) {
    setProvider("text", provider);
    return *this;
}

// ── PilotStat ──

PilotStat::PilotStat(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "stat") {
    put("label", PilotJson(label));
}

PilotStat& PilotStat::value(const PilotString& v) { put("value", PilotJson(v)); return *this; }
PilotStat& PilotStat::unit(const PilotString& u) { put("unit", PilotJson(u)); return *this; }

PilotStat& PilotStat::valueProvider(PilotValueProvider* provider) {
    setProvider("value", provider);
    return *this;
}

// ── PilotTable ──

PilotTable::PilotTable(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "table") {
    put("label", PilotJson(label));
}

PilotTable& PilotTable::columns(const PilotVector<PilotPair<PilotString, PilotString>>& cols) {
    PilotJson arr = PilotJson::array();
    for (const auto& col : cols) {
        arr.add(PilotJson::object()
            .put("key", PilotJson(col.first))
            .put("label", PilotJson(col.second)));
    }
    put("columns", arr);
    return *this;
}

PilotTable& PilotTable::rows(const PilotVector<PilotJson>& rowData) {
    PilotJson arr = PilotJson::array();
    for (const auto& row : rowData) {
        arr.add(row);
    }
    put("rows", arr);
    return *this;
}

// ── PilotLogs ──

PilotLogs::PilotLogs(PilotUI& ui, const PilotString& label)
    : PilotWidget(ui, "logs") {
    put("label", PilotJson(label));
}

PilotLogs& PilotLogs::maxLines(int v) { put("maxLines", PilotJson(v)); return *this; }

} // namespace Pilot
