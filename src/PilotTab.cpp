#include "Pilot/PilotTab.h"
#include "Pilot/PilotUI.h"

namespace Pilot {

PilotTab::PilotTab(PilotUI& ui, const PilotString& title)
    : m_ui(ui)
    , m_title(title)
    , m_internalId(ui.nextId())
    , m_id(PilotString("tab-") + pilotToString(m_internalId)) {
}

PilotLayout& PilotTab::vertical() {
    m_layout = std::make_unique<PilotLayout>(m_ui, PilotLayout::Direction::VERTICAL);
    return *m_layout;
}

PilotLayout& PilotTab::horizontal() {
    m_layout = std::make_unique<PilotLayout>(m_ui, PilotLayout::Direction::HORIZONTAL);
    return *m_layout;
}

PilotJson PilotTab::toJson() const {
    PilotJson json = PilotJson::object();
    json.put("id", PilotJson(m_internalId));
    json.put("title", PilotJson(m_title));
    if (m_layout) {
        json.put("layout", m_layout->toJson());
    }
    return json;
}

} // namespace Pilot
