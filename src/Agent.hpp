#ifndef ORGANIZATION_MODEL_AGENT_HPP
#define ORGANIZATION_MODEL_AGENT_HPP

#include "AtomicAgent.hpp"
#include "OrganizationModelAsk.hpp"
#include "policies/EnergyProviderPolicy.hpp"
#include "policies/TransportProviderPolicy.hpp"

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
     * Get the robot facade
     * \return facade object
     */
    facades::Robot getFacade(const OrganizationModelAsk& ask) const;

    /**
     * Get the set of atomic agents
     */
    const AtomicAgent::Set& getAtomicAgents() const { return mAtomicAgents; }

    /**
     * Get the relative energy consumption for atomic agents
     */
    const std::map<AtomicAgent, double>& getEnergyProviderShares() const { return mEnergyProviderPolicy.getShares(); }

    bool operator<(const Agent& other) const { return this->mAtomicAgents < other.mAtomicAgents; }
    bool operator==(const Agent& other) const { return this->mAtomicAgents == other.mAtomicAgents; }
    size_t size() const { return mAtomicAgents.size(); }

    /**
     * Stringify agent
     * \return string
     */
    std::string toString(size_t indent = 0) const;

    /**
     * Get the list of all atomic agents that are part of the agent set
     */
    static AtomicAgent::Set allAtomicAgents(const Agent::Set& agents);

    /**
     * Compute intersection with other agent
     * \return intersection of atomic agents
     */
    AtomicAgent::List getIntersection(const Agent& other) const;

    /**
     * Check if this agent and other agent share atomic agents
     */
    bool intersectsWith(const Agent& other) const;

    /**
     * Get the difference between two agents
     */
    AtomicAgent::List getDifference(const Agent& other) const;

private:
    AtomicAgent::Set mAtomicAgents;
    policies::EnergyProviderPolicy mEnergyProviderPolicy;
    policies::TransportProviderPolicy mTransportProviderPolicy;

};

typedef Agent::List CoalitionStructure;

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_AGENT_HPP
