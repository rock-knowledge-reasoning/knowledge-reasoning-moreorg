#include "TransportProviderPolicy.hpp"

#include "../Agent.hpp"

namespace organization_model {
namespace policies {

const AtomicAgent::Set& TransportProviderPolicy::activeTransportProviders(const Agent& agent, const organization_model::OrganizationModelAsk& ask) const
{
    std::map<Agent, AtomicAgent::Set>::const_iterator cit = mActiveTransportProviders.find(agent);
    if(cit != mActiveTransportProviders.end())
    {
        return cit->second;
    }

    AtomicAgent::Set activeTransportAgents;

    double maxCapacity = std::numeric_limits<double>::min();
    AtomicAgent activeAgent;
    // identify active transport system
    for(const AtomicAgent& aa : agent.getAtomicAgents())
    {
        double capacity = aa.getFacade(ask).getTransportCapacity();
        if(capacity > maxCapacity)
        {
            maxCapacity = capacity;
            activeAgent = aa;
        }
    }
    activeTransportAgents.insert(activeAgent);

    mActiveTransportProviders[agent] = activeTransportAgents;
    return mActiveTransportProviders[agent];
}

} // end namespace policies
} // end namespace organization_model
