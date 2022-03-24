#include "Agent.hpp"

using namespace owlapi::model;

namespace moreorg {

std::map<Agent::OperationalStatus, std::string> Agent::OperationalStatusTxt = {
    {Agent::UNKNOWN_OPERATIONAL_STATUS, "unknown operational status"},
    {Agent::OPERATIVE, "operative"},
    {Agent::DORMANT, "dormant"}};

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

Agent::Agent(const ModelPool& modelPool)
    : mAtomicAgents()
{
    for(const std::pair<const IRI, size_t>& p : modelPool)
    {
        for(size_t i = 0; i < p.second; ++i)
        {
            AtomicAgent a(i, p.first);
            add(a);
        }
    }
}

facades::Robot Agent::getFacade(const OrganizationModelAsk& ask) const
{
    ModelPool modelPool = getType();
    return facades::Robot::getInstance(modelPool, ask);
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

std::string Agent::toString(size_t indent) const
{
    std::string hspace(indent, ' ');
    std::stringstream ss;
    ss << hspace << "Agent:" << std::endl;
    for(const AtomicAgent& aa : mAtomicAgents)
    {
        ss << hspace << "    " << aa.toString() << std::endl;
    }
    return ss.str();
}

std::string Agent::toString(const List& agents, size_t indent)
{
    std::string hspace(indent, ' ');
    std::stringstream ss;
    ss << hspace << "Agents:" << std::endl;
    for(const Agent& a : agents)
    {
        ss << a.toString(indent + 4);
    }
    return ss.str();
}

std::string Agent::toString(const Set& agents, size_t indent)
{
    std::string hspace(indent, ' ');
    std::stringstream ss;
    ss << hspace << "Agents:" << std::endl;
    for(const Agent& a : agents)
    {
        ss << a.toString(indent + 4);
    }
    return ss.str();
}

AtomicAgent::Set Agent::allAtomicAgents(const Agent::Set& agents)
{
    AtomicAgent::Set atomicAgents;
    for(const Agent& agent : agents)
    {
        atomicAgents.insert(agent.getAtomicAgents().begin(),
                            agent.getAtomicAgents().end());
    }
    return atomicAgents;
}

AtomicAgent::List Agent::getIntersection(const Agent& other) const
{
    return AtomicAgent::getIntersection(mAtomicAgents, other.mAtomicAgents);
}

bool Agent::intersectsWith(const Agent& other) const
{
    return !AtomicAgent::getIntersection(mAtomicAgents, other.mAtomicAgents)
                .empty();
}

AtomicAgent::List Agent::getDifference(const Agent& other) const
{
    return AtomicAgent::getDifference(mAtomicAgents, other.mAtomicAgents);
}

std::ostream& operator<<(std::ostream& os, const Agent::Set& agents)
{
    for(const Agent& a : agents)
    {
        os << a.toString() << std::endl;
    }
    return os;
}

} // end namespace moreorg
