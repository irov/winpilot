#pragma once

#include <Pilot/PilotJson.h>
#include <Pilot/PilotAction.h>
#include <Pilot/PilotWidgetCallback.h>

namespace Pilot {

class PilotTab;
class PilotWidget;

class PilotUI {
public:
    PilotUI();
    ~PilotUI();

    int nextId();

    // Tab management
    PilotTab& addTab(const PilotString& title);
    PilotTab* getTab(const PilotString& id);
    void removeTab(const PilotString& id);

    // Widget callbacks
    void registerCallback(int widgetId, PilotWidgetCallback* callback);
    void registerCallbackFunc(int widgetId, PilotWidgetCallbackFunc func);
    void removeCallback(int widgetId);
    bool dispatchAction(const PilotAction& action);

    // Value providers
    void registerProvider(PilotWidget* widget);
    void unregisterProvider(PilotWidget* widget);
    void pollValues();

    // Revision tracking
    void incrementRevision();
    bool hasUnsent() const;
    void markSent();
    bool hasTabs() const;
    int getRevision() const;

    // Serialization
    PilotJson toJson();

private:
    PilotVector<PilotUniquePtr<PilotTab>> m_tabs;
    PilotMutex m_tabMutex;

    struct CallbackEntry {
        PilotWidgetCallback* raw = nullptr;
        PilotWidgetCallbackFunc func;
    };
    PilotUnorderedMap<int, CallbackEntry> m_callbacks;
    PilotMutex m_callbackMutex;

    PilotVector<PilotWidget*> m_providers;
    PilotMutex m_providerMutex;

    PilotAtomic<int> m_idCounter{0};
    PilotAtomic<int> m_revision{1};
    int m_sentRevision = 0;
    static const int VERSION = 2;
};

} // namespace Pilot
