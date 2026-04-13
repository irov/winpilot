#include <Pilot/PilotUI.h>
#include <Pilot/PilotTab.h>
#include <Pilot/PilotWidget.h>
#include <Pilot/PilotLog.h>

#include <algorithm>

namespace Pilot {

PilotUI::PilotUI() = default;
PilotUI::~PilotUI() = default;

int PilotUI::nextId() {
    return ++m_idCounter;
}

// ── Tab management ──

PilotTab& PilotUI::addTab(const PilotString& title) {
    std::lock_guard<PilotMutex> lock(m_tabMutex);
    // Remove existing tab with same title
    m_tabs.erase(
        std::remove_if(m_tabs.begin(), m_tabs.end(),
            [&title](const PilotUniquePtr<PilotTab>& t) { return t->title() == title; }),
        m_tabs.end());

    auto tab = std::make_unique<PilotTab>(*this, title);
    auto* ptr = tab.get();
    m_tabs.push_back(std::move(tab));
    m_revision++;
    return *ptr;
}

PilotTab* PilotUI::getTab(const PilotString& id) {
    std::lock_guard<PilotMutex> lock(m_tabMutex);
    for (auto& t : m_tabs) {
        if (t->id() == id) return t.get();
    }
    return nullptr;
}

void PilotUI::removeTab(const PilotString& id) {
    std::lock_guard<PilotMutex> lock(m_tabMutex);
    m_tabs.erase(
        std::remove_if(m_tabs.begin(), m_tabs.end(),
            [&id](const PilotUniquePtr<PilotTab>& t) { return t->id() == id; }),
        m_tabs.end());
    m_revision++;
}

// ── Widget callbacks ──

void PilotUI::registerCallback(int widgetId, PilotWidgetCallback* callback) {
    std::lock_guard<PilotMutex> lock(m_callbackMutex);
    if (callback) {
        CallbackEntry entry;
        entry.raw = callback;
        m_callbacks[widgetId] = entry;
    } else {
        m_callbacks.erase(widgetId);
    }
}

void PilotUI::registerCallbackFunc(int widgetId, PilotWidgetCallbackFunc func) {
    std::lock_guard<PilotMutex> lock(m_callbackMutex);
    if (func) {
        CallbackEntry entry;
        entry.func = std::move(func);
        m_callbacks[widgetId] = entry;
    } else {
        m_callbacks.erase(widgetId);
    }
}

void PilotUI::removeCallback(int widgetId) {
    std::lock_guard<PilotMutex> lock(m_callbackMutex);
    m_callbacks.erase(widgetId);
}

bool PilotUI::dispatchAction(const PilotAction& action) {
    std::lock_guard<PilotMutex> lock(m_callbackMutex);
    auto it = m_callbacks.find(action.widgetId());
    if (it == m_callbacks.end()) return false;

    const auto& entry = it->second;
    if (entry.func) {
        entry.func(action);
    } else if (entry.raw) {
        entry.raw->onPilotWidgetAction(action);
    }
    return true;
}

// ── Value providers ──

void PilotUI::registerProvider(PilotWidget* widget) {
    std::lock_guard<PilotMutex> lock(m_providerMutex);
    if (std::find(m_providers.begin(), m_providers.end(), widget) == m_providers.end()) {
        m_providers.push_back(widget);
    }
}

void PilotUI::unregisterProvider(PilotWidget* widget) {
    std::lock_guard<PilotMutex> lock(m_providerMutex);
    m_providers.erase(
        std::remove(m_providers.begin(), m_providers.end(), widget),
        m_providers.end());
}

void PilotUI::pollValues() {
    std::lock_guard<PilotMutex> lock(m_providerMutex);
    for (auto* w : m_providers) {
        if (w->pollProvider()) {
            m_revision++;
        }
    }
}

// ── Revision tracking ──

void PilotUI::incrementRevision() {
    m_revision++;
}

bool PilotUI::hasUnsent() const {
    return m_revision.load() != m_sentRevision;
}

void PilotUI::markSent() {
    m_sentRevision = m_revision.load();
}

bool PilotUI::hasTabs() const {
    return !m_tabs.empty();
}

int PilotUI::getRevision() const {
    return m_revision.load();
}

// ── Serialization ──

PilotJson PilotUI::toJson() {
    std::lock_guard<PilotMutex> lock(m_tabMutex);
    PilotJson json = PilotJson::object();
    json.put("version", PilotJson(VERSION));
    json.put("revision", PilotJson(m_revision.load()));

    PilotJson tabsArr = PilotJson::array();
    for (const auto& tab : m_tabs) {
        tabsArr.add(tab->toJson());
    }
    json.put("tabs", tabsArr);
    return json;
}

} // namespace Pilot
