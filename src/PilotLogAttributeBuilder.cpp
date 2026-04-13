#include "Pilot/PilotLogAttributeBuilder.h"

namespace Pilot {

PilotLogAttributeBuilder& PilotLogAttributeBuilder::put(const PilotString& key, const PilotString& value) {
    m_staticAttributes[key] = value;
    return *this;
}

PilotLogAttributeBuilder& PilotLogAttributeBuilder::putProvider(const PilotString& key, PilotValueProvider* provider) {
    m_dynamicAttributes[key] = provider;
    return *this;
}

const PilotMap<PilotString, PilotString>& PilotLogAttributeBuilder::staticAttributes() const { return m_staticAttributes; }
const PilotMap<PilotString, PilotValueProvider*>& PilotLogAttributeBuilder::dynamicAttributes() const { return m_dynamicAttributes; }

} // namespace Pilot
