#include "AgentInterface.hpp"

namespace moreorg {

const AtomicAgent& AgentInterface::getAgent() const { return mAgent; }

AgentInterface::AgentInterface(const AtomicAgent& agent,
                               const owlapi::model::IRI& interface)
    : mAgent(agent)
    , mInterface(interface)
{
}

} // end namespace moreorg
