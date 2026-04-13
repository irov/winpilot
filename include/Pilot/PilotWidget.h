#pragma once

#include "Pilot/PilotLayoutChild.h"
#include "Pilot/PilotJson.h"
#include "Pilot/PilotValueProvider.h"

namespace Pilot {

class PilotUI;

class PilotWidget : public PilotLayoutChild {
public:
    PilotWidget(PilotUI& ui, const PilotString& type);
    ~PilotWidget() override = default;

    int internalId() const { return m_internalId; }
    const PilotString& publicId() const { return m_publicId; }
    void setId(const PilotString& id) { m_publicId = id; }
    const PilotString& widgetType() const { return m_type; }

    bool pollProvider();
    PilotJson toJson() const override { return m_json; }

protected:
    void put(const PilotString& key, const PilotJson& value);
    void setProvider(const PilotString& key, PilotValueProvider* provider);

    PilotUI& m_ui;

private:
    int m_internalId;
    PilotString m_publicId;
    PilotString m_type;
    PilotJson m_json;

    PilotString m_providerKey;
    PilotValueProvider* m_provider = nullptr;
    PilotString m_cachedValue;
};

} // namespace Pilot
