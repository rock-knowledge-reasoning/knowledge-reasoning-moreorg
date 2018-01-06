#ifndef ORGANIZATION_MODEL_AGENT_HPP
#define ORGANIZATION_MODEL_AGENT_HPP

#include "AtomicAgent.hpp"
#include "OrganizationModelAsk.hpp"
#include "policies/EnergyProviderPolicy.hpp"

namespace organization_model {

/**
 * \class Agent
 * \brief General agent representation
 */
class Agent
{
public:
    typedef std::vector<Agent> List;
    typedef std::set<Agent> Set;

    /**
     * Default constructor to allow lists and map usage
     */
    Agent();
    Agent(const AtomicAgent& atomicAgent);
    Agent(const AtomicAgent::List& atomicAgents);
    Agent(const AtomicAgent::Set& atomicAgents);

    /**
     * Add an atomic agent to this agent object
     */
    void add(const AtomicAgent& agent) { mAtomicAgents.insert(agent); }

    /**
     * Update internal structures, such as energy consumption share when the
     * agent is complete
     */
    void update(const OrganizationModelAsk& ask);

    /**
     * Test if this is an atomic agent, i.e. |A| = 1
     */
    bool isAtomic() const { return mAtomicAgents.size() == 1; }

    /**
     * Test if this is a composite agent, i.e. |A| > 1
     */
    bool isComposite() const { return mAtomicAgents.size() > 1; }

    /**
     * Test if this is a null agent, i.e. A = {} or equivalently |A| = 0
     */
    bool isNull() const { return mAtomicAgents.empty(); }

    /**
     * The agent type as model pool
     * \return model pool as agent type
     */
    ModelPool getType() const;

    /**
     * Get the robot facet
     * \return facet object
     */
    facets::Robot getFacet(const OrganizationModelAsk& ask) const;

    /**
     * Get the set of atomic agents
     */
    const AtomicAgent::Set& getAtomicAgents() const { return mAtomicAgents; }

    /**
     * Get the relative energy consumption for atomic agents
     */
    const std::map<AtomicAgent, double>& getEnergyProviderShares() const { return mEnergyProviderPolicy.getShares(); }

private:
    AtomicAgent::Set mAtomicAgents;
    policies::EnergyProviderPolicy mEnergyProviderPolicy;

};

typedef Agent::List CoalitionStructure;

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_AGENT_HPP
