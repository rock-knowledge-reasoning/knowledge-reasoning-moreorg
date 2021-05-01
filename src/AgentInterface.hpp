#ifndef ORGANIZATION_MODEL_AGENT_INTERFACE_HPP
#define ORGANIZATION_MODEL_AGENT_INTERFACE_HPP

#include "AtomicAgent.hpp"

namespace moreorg {

class AgentInterface
{
public:
    AgentInterface() = default;

    AgentInterface(const AtomicAgent& agent,
            const owlapi::model::IRI& interface);

    const AtomicAgent& getAgent() const;

    const owlapi::model::IRI& getInterface() const { return mInterface; }

private:
    AtomicAgent mAgent;
    owlapi::model::IRI mInterface;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_AGENT_INTERFACE_HPP
