#include <Pilot/PilotLayout.h>
#include <Pilot/PilotUI.h>
#include <Pilot/PilotButton.h>
#include <Pilot/PilotLabel.h>
#include <Pilot/PilotStat.h>
#include <Pilot/PilotSwitch.h>
#include <Pilot/PilotInput.h>
#include <Pilot/PilotSelect.h>
#include <Pilot/PilotTextarea.h>
#include <Pilot/PilotTable.h>
#include <Pilot/PilotLogs.h>

namespace Pilot {

PilotLayout::PilotLayout(PilotUI& ui, Direction dir)
    : m_ui(ui), m_direction(dir) {
}

// ── Sub-layouts ──

PilotLayout& PilotLayout::addVertical() {
    auto sub = std::make_unique<PilotLayout>(m_ui, Direction::VERTICAL);
    auto* ptr = sub.get();
    m_children.push_back(std::move(sub));
    m_ui.incrementRevision();
    return *ptr;
}

PilotLayout& PilotLayout::addHorizontal() {
    auto sub = std::make_unique<PilotLayout>(m_ui, Direction::HORIZONTAL);
    auto* ptr = sub.get();
    m_children.push_back(std::move(sub));
    m_ui.incrementRevision();
    return *ptr;
}

PilotLayout& PilotLayout::addCollapsible(const PilotString& title) {
    auto collapsible = std::make_unique<CollapsibleElement>(m_ui, title);
    auto& content = collapsible->content();
    m_children.push_back(std::move(collapsible));
    m_ui.incrementRevision();
    return content;
}

// ── Padding ──

PilotLayout& PilotLayout::addPadding(double weight) {
    m_children.push_back(std::make_unique<PaddingElement>(weight));
    m_ui.incrementRevision();
    return *this;
}

// ── Widgets ──

PilotButton& PilotLayout::addButton(const PilotString& label) {
    auto w = std::make_unique<PilotButton>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotLabel& PilotLayout::addLabel(const PilotString& text) {
    auto w = std::make_unique<PilotLabel>(m_ui, text);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotStat& PilotLayout::addStat(const PilotString& label) {
    auto w = std::make_unique<PilotStat>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotSwitch& PilotLayout::addSwitch(const PilotString& label) {
    auto w = std::make_unique<PilotSwitch>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotInput& PilotLayout::addInput(const PilotString& label) {
    auto w = std::make_unique<PilotInput>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotSelect& PilotLayout::addSelect(const PilotString& label) {
    auto w = std::make_unique<PilotSelect>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotTextarea& PilotLayout::addTextarea(const PilotString& label) {
    auto w = std::make_unique<PilotTextarea>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotTable& PilotLayout::addTable(const PilotString& label) {
    auto w = std::make_unique<PilotTable>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

PilotLogs& PilotLayout::addLogs(const PilotString& label) {
    auto w = std::make_unique<PilotLogs>(m_ui, label);
    auto* ptr = w.get();
    m_children.push_back(std::move(w));
    m_ui.incrementRevision();
    return *ptr;
}

// ── Serialization ──

PilotJson PilotLayout::toJson() const {
    PilotJson json = PilotJson::object();
    json.put("type", PilotJson("layout"));
    json.put("direction", PilotJson(m_direction == Direction::VERTICAL ? "vertical" : "horizontal"));

    PilotJson childrenArr = PilotJson::array();
    for (const auto& child : m_children) {
        childrenArr.add(child->toJson());
    }
    json.put("children", childrenArr);
    return json;
}

} // namespace Pilot
