#include "TransportProviderPolicy.hpp"

#include "../Agent.hpp"
#include "../vocabularies/OM.hpp"
#include "../facades/Robot.hpp"

namespace moreorg {
namespace policies {

PolicyRegistration<TransportProviderPolicy> TransportProviderPolicy::msRegistration;

TransportProviderPolicy::TransportProviderPolicy()
    : SelectionPolicy(vocabulary::OM::resolve("TransportProviderPolicy"))
{
}

TransportProviderPolicy::TransportProviderPolicy(const ModelPool& modelPool, const moreorg::OrganizationModelAsk& ask)
    : SelectionPolicy(modelPool, ask, vocabulary::OM::resolve("TransportProviderPolicy"))
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
        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        if(robot.isMobile() && robot.getTransportCapacity() > 0)
        {
            mSelection.agent = modelPool;
        }
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
        mSelection.agent[activeAgent] = 1;
    }
}

} // end namespace policies
} // end namespace moreorg
