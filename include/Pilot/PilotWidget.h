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

    int internalId() const;
    const PilotString& publicId() const;
    void setId(const PilotString& id);
    const PilotString& widgetType() const;

    bool pollProvider();
    PilotJson toJson() const override;

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
