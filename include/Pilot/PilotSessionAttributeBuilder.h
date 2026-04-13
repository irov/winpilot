#pragma once

#include <Pilot/PilotJson.h>
#include <Pilot/PilotValueProvider.h>

namespace Pilot {

class PilotSessionAttributeBuilder {
public:
    PilotSessionAttributeBuilder& put(const PilotString& key, const PilotJson& value) {
        m_staticAttributes[key] = value;
        return *this;
    }

    PilotSessionAttributeBuilder& putProvider(const PilotString& key, PilotValueProvider* provider) {
        m_dynamicAttributes[key] = provider;
        return *this;
    }

    const PilotMap<PilotString, PilotJson>& staticAttributes() const { return m_staticAttributes; }
    const PilotMap<PilotString, PilotValueProvider*>& dynamicAttributes() const { return m_dynamicAttributes; }

private:
    PilotMap<PilotString, PilotJson> m_staticAttributes;
    PilotMap<PilotString, PilotValueProvider*> m_dynamicAttributes;
};

} // namespace Pilot
