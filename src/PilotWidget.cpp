#include "Pilot/PilotWidget.h"
#include "Pilot/PilotUI.h"
#include "Pilot/PilotLog.h"

namespace Pilot {

PilotWidget::PilotWidget(PilotUI& ui, const PilotString& type)
    : m_ui(ui)
    , m_internalId(ui.nextId())
    , m_publicId(type + PilotString("-") + pilotToString(m_internalId))
    , m_type(type)
    , m_json(PilotJson::object()) {
    m_json.put("type", PilotJson(type));
    m_json.put("id", PilotJson(m_internalId));
}

int PilotWidget::internalId() const {
    return m_internalId;
}

const PilotString& PilotWidget::publicId() const {
    return m_publicId;
}

void PilotWidget::setId(const PilotString& id) {
    m_publicId = id;
}

const PilotString& PilotWidget::widgetType() const {
    return m_type;
}

PilotJson PilotWidget::toJson() const {
    return m_json;
}

void PilotWidget::put(const PilotString& key, const PilotJson& value) {
    m_json.put(key, value);
    m_ui.incrementRevision();
}

void PilotWidget::setProvider(const PilotString& key, PilotValueProvider* provider) {
    m_providerKey = key;
    m_provider = provider;
    if (provider) {
        m_ui.registerProvider(this);
    } else {
        m_ui.unregisterProvider(this);
    }
}

bool PilotWidget::pollProvider() {
    if (!m_provider) return false;
    try {
        PilotString newValue = m_provider->getValue();
        if (newValue != m_cachedValue) {
            m_cachedValue = newValue;
            if (!m_providerKey.empty()) {
                m_json.put(m_providerKey, PilotJson(newValue));
            }
            return true;
        }
    } catch (...) {
        PilotLog::e("Value provider failed for widget %d", m_internalId);
    }
    return false;
}

} // namespace Pilot
