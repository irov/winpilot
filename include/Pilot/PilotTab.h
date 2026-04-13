#pragma once

#include <Pilot/PilotLayout.h>
#include <Pilot/PilotJson.h>

namespace Pilot {

class PilotUI;

class PilotTab {
public:
    PilotTab(PilotUI& ui, const PilotString& title);

    PilotTab& setId(const PilotString& id) { m_id = id; return *this; }

    PilotLayout& vertical();
    PilotLayout& horizontal();

    const PilotString& title() const { return m_title; }
    const PilotString& id() const { return m_id; }
    int internalId() const { return m_internalId; }
    bool hasLayout() const { return m_layout != nullptr; }

    PilotJson toJson() const;

private:
    PilotUI& m_ui;
    PilotString m_title;
    PilotString m_id;
    int m_internalId;
    PilotUniquePtr<PilotLayout> m_layout;
};

} // namespace Pilot
