#ifndef ORGANIZATION_MODEL_AGENT_HPP
#define ORGANIZATION_MODEL_AGENT_HPP

#include "AtomicAgent.hpp"
#include "OrganizationModelAsk.hpp"

namespace organization_model {

/**
 * \class Agent
 * \brief General agent representation
 * \details The general agent consists of the composition of multiple atomic
 * agents. This class provides methods in order to work with agents.
 * The method \a update has to be called when the agent has been updated in
 * order to refresh the setting of the internal policies
 */
class Agent
{
public:
    typedef std::vector<Agent> List;
    typedef std::set<Agent> Set;

    /// The operational status of a agent
    enum OperationalStatus { UNKNOWN_OPERATIONAL_STATUS = 0, OPERATIVE, DORMANT };

    static std::map<OperationalStatus, std::string> OperationalStatusTxt;

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

    bool operator<(const Agent& other) const { return this->mAtomicAgents < other.mAtomicAgents; }
    bool operator==(const Agent& other) const { return this->mAtomicAgents == other.mAtomicAgents; }
    size_t size() const { return mAtomicAgents.size(); }

    /**
     * Stringify agent
     * \return string
     */
    std::string toString(size_t indent = 0) const;
    static std::string toString(const List& agents, size_t indent = 0);
    static std::string toString(const Set& agents, size_t indent = 0);

    /**
     * Get the list of all atomic agents that are part of the agent set
     * \return return all atomic agents that are part of a set of (composite) agents
     */
    static AtomicAgent::Set allAtomicAgents(const Agent::Set& agents);

    /**
     * Compute intersection with other agent (considering the set of atomic * agents)
     * \return intersection of atomic agents
     */
    AtomicAgent::List getIntersection(const Agent& other) const;

    /**
     * Check if this agent and other agent share atomic agents
     * \return true if this agent shares atomic agents with other, false
     * otherwise
     */
    bool intersectsWith(const Agent& other) const;

    /**
     * Get the difference between two agents in terms of atomic agents
     */
    AtomicAgent::List getDifference(const Agent& other) const;

private:
    AtomicAgent::Set mAtomicAgents;
};

/// A coalition structure is a list of (composite) agents
typedef Agent::List CoalitionStructure;

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_AGENT_HPP
