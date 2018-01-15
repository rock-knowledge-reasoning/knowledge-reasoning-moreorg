#include "EnergyProviderPolicy.hpp"
#include "../Agent.hpp"
#include <iostream>

namespace organization_model {
namespace policies {

void EnergyProviderPolicy::update(const Agent& agent, const OrganizationModelAsk& ask)
{
    // set energy consumption policy
    //
    double fullCapacity = 0;
    std::map<AtomicAgent, double> energyCapacity;

    for(const AtomicAgent& atomicAgent : agent.getAtomicAgents())
    {
        facades::Robot robot = atomicAgent.getFacade(ask);
        double capacity = robot.getEnergyCapacity();
        fullCapacity += capacity;
        energyCapacity[atomicAgent] = capacity;
    }

    for(const std::pair<AtomicAgent,double>& p : energyCapacity)
    {
        mEnergyProviderShares[p.first] = p.second / fullCapacity;
    }
}

} // end namespace policies
} // end organization_model
