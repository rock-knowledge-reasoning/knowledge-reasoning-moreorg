#include "TransportProviderPolicy.hpp"

#include "../Agent.hpp"

namespace moreorg {
namespace policies {

TransportProviderPolicy::TransportProviderPolicy()
    : Policy()
{
}

TransportProviderPolicy::TransportProviderPolicy(const ModelPool& modelPool, const moreorg::OrganizationModelAsk& ask)
    : Policy(modelPool, ask)
{
    update(modelPool, ask);
}

TransportProviderPolicy::~TransportProviderPolicy()
{}


void TransportProviderPolicy::update(const ModelPool& modelPool, const OrganizationModelAsk& ask)
{
    ModelPool activeTransportAgents;

    if(modelPool.empty())
    {
        throw
            std::invalid_argument("owlapi::model::TransportProviderPolicy::update "
                    " invalid argument: model pool cannot be empty");
    } else if(modelPool.numberOfInstances() == 1)
    {
        mActiveTransportProviders = modelPool;
    } else {
        // Assume there can be only one active transport provider
        double maxCapacity = std::numeric_limits<double>::min();
        owlapi::model::IRI activeAgent;
        // identify active transport system
        for(const ModelPool::value_type p : modelPool)
        {
            const owlapi::model::IRI& agentType = p.first;
            double capacity = facades::Robot::getInstance(agentType, ask).getTransportCapacity();
            if(capacity > maxCapacity)
            {
                maxCapacity = capacity;
                activeAgent = agentType;
            }
        }
        mActiveTransportProviders[activeAgent] = 1;
    }
}

} // end namespace policies
} // end namespace moreorg
