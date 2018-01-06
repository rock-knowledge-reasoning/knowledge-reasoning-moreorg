#include "Agent.hpp"

namespace organization_model {

Agent::Agent()
{}

Agent::Agent(const AtomicAgent& atomicAgent)
{
    mAtomicAgents.insert(atomicAgent);
}

Agent::Agent(const AtomicAgent::List& atomicAgents)
    : mAtomicAgents(atomicAgents.begin(), atomicAgents.end())
{
}

Agent::Agent(const AtomicAgent::Set& atomicAgents)
    : mAtomicAgents(atomicAgents)
{
}

facets::Robot Agent::getFacet(const OrganizationModelAsk& ask) const
{
    ModelPool modelPool = getType();
    return facets::Robot::getInstance(modelPool, ask);
}

void Agent::update(const OrganizationModelAsk& ask)
{
    mEnergyProviderPolicy.update(*this, ask);
}

ModelPool Agent::getType() const
{
    ModelPool pool;
    for(const AtomicAgent& agent : mAtomicAgents)
    {
        pool[agent.getModel()] += 1;
    }
    return pool;
}

}
