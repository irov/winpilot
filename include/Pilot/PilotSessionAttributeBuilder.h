#pragma once

#include "Pilot/PilotJson.h"
#include "Pilot/PilotValueProvider.h"

namespace Pilot {

class PilotSessionAttributeBuilder {
public:
    PilotSessionAttributeBuilder& put(const PilotString& key, const PilotJson& value);
    PilotSessionAttributeBuilder& putProvider(const PilotString& key, PilotValueProvider* provider);

    const PilotMap<PilotString, PilotJson>& staticAttributes() const;
    const PilotMap<PilotString, PilotValueProvider*>& dynamicAttributes() const;

private:
    PilotMap<PilotString, PilotJson> m_staticAttributes;
    PilotMap<PilotString, PilotValueProvider*> m_dynamicAttributes;
};

} // namespace Pilot
