#pragma once

#include "Pilot/PilotValueProvider.h"

namespace Pilot {

class PilotLogAttributeBuilder {
public:
    PilotLogAttributeBuilder& put(const PilotString& key, const PilotString& value);
    PilotLogAttributeBuilder& putProvider(const PilotString& key, PilotValueProvider* provider);

    const PilotMap<PilotString, PilotString>& staticAttributes() const;
    const PilotMap<PilotString, PilotValueProvider*>& dynamicAttributes() const;

private:
    PilotMap<PilotString, PilotString> m_staticAttributes;
    PilotMap<PilotString, PilotValueProvider*> m_dynamicAttributes;
};

} // namespace Pilot
