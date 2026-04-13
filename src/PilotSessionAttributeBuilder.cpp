#include "Pilot/PilotSessionAttributeBuilder.h"

namespace Pilot {

PilotSessionAttributeBuilder& PilotSessionAttributeBuilder::put(const PilotString& key, const PilotJson& value) {
    m_staticAttributes[key] = value;
    return *this;
}

PilotSessionAttributeBuilder& PilotSessionAttributeBuilder::putProvider(const PilotString& key, PilotValueProvider* provider) {
    m_dynamicAttributes[key] = provider;
    return *this;
}

const PilotMap<PilotString, PilotJson>& PilotSessionAttributeBuilder::staticAttributes() const { return m_staticAttributes; }
const PilotMap<PilotString, PilotValueProvider*>& PilotSessionAttributeBuilder::dynamicAttributes() const { return m_dynamicAttributes; }

} // namespace Pilot
