#include "AtomicAgent.hpp"

#include <sstream>
#include "OrganizationModelAsk.hpp"

namespace moreorg {

AtomicAgent::AtomicAgent()
    : mModel()
    , mId(0)
    , mName("unknown")
{}

AtomicAgent::AtomicAgent(size_t id, const owlapi::model::IRI& model)
    : mModel(model)
    , mId(id)
{
    std::stringstream ss;
    ss << mModel.getFragment() << "_" << mId;
    mName = ss.str();
}

std::string AtomicAgent::toString() const
{
    std::stringstream ss;
    ss << "AtomicAgent: " << getName() << " (";
    if(mModel != owlapi::model::IRI())
    {
        ss << mModel.getFragment();
    } else {
        ss << mModel;
    }
    ss << ")";
    return ss.str();
}

std::string AtomicAgent::toString(const AtomicAgent::List& agents, size_t indent)
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    if(agents.empty())
    {
        ss << hspace << "AtomicAgents: empty " << std::endl;
        return ss.str();
    }

    AtomicAgent::List::const_iterator cit = agents.begin();
    ss << hspace << "AtomicAgents: " << std::endl;
    for(; cit != agents.end(); ++cit)
    {
        ss << hspace << "    - " << cit->toString() << std::endl;
    }
    return ss.str();
}

std::string AtomicAgent::toString(const AtomicAgent::Set& agents, size_t indent)
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    if(agents.empty())
    {
        ss << hspace << "AtomicAgents: empty " << std::endl;
        return ss.str();
    }

    ss << hspace << "AtomicAgents: " << std::endl;
    AtomicAgent::Set::const_iterator cit = agents.begin();
    for(; cit != agents.end(); ++cit)
    {
        ss << hspace << "    - " << cit->toString() << std::endl;
    }
    return ss.str();
}

moreorg::ModelPool AtomicAgent::getModelPool(const AtomicAgent::List& agents)
{
    moreorg::ModelPool modelPool;
    for(const AtomicAgent& agent : agents)
    {
        owlapi::model::IRI model = agent.getModel();
        modelPool[model] +=1;
    }
    return modelPool;
}

moreorg::ModelPool AtomicAgent::getModelPool(const AtomicAgent::Set& agents)
{
    moreorg::ModelPool modelPool;
    for(const AtomicAgent& agent : agents)
    {
        owlapi::model::IRI model = agent.getModel();
        modelPool[model] +=1;
    }
    return modelPool;
}

AtomicAgent::List AtomicAgent::getIntersection(const AtomicAgent::Set& a0,
        const AtomicAgent::Set& a1)
{
     std::vector<AtomicAgent> intersection;
     std::set_intersection(a0.begin(), a0.end(),
             a1.begin(), a1.end(),
             std::back_inserter(intersection)
     );
     return intersection;
}

AtomicAgent::List AtomicAgent::getDifference(const AtomicAgent::Set& a0,
        const AtomicAgent::Set& a1)
{
    // computing remaining, i.e.  remainingInA0 = from - to
    std::vector<AtomicAgent> remainingInA0;

    std::set_difference(a0.begin(), a0.end(),
            a1.begin(), a1.end(),
            std::inserter(remainingInA0, remainingInA0.begin()));

    return remainingInA0;
}

bool AtomicAgent::operator<(const AtomicAgent& other) const
{
    if(mModel == other.mModel)
    {
        return mId < other.mId;
    }
    return mModel < other.mModel;
}

AtomicAgent::List AtomicAgent::toList(const moreorg::ModelPool& modelPool)
{
    List agents;
    moreorg::ModelPool::const_iterator cit = modelPool.cbegin();
    for(;cit != modelPool.cend(); ++cit)
    {
        const owlapi::model::IRI& model = cit->first;
        size_t count = cit->second;

        // Update agents
        for(size_t i = 0; i < count; ++i)
        {
            AtomicAgent agent(i, model);
            agents.push_back(agent);
        }
    }
    return agents;
}

facades::Robot AtomicAgent::getFacade(const OrganizationModelAsk& ask) const
{
    return facades::Robot::getInstance(mModel, ask);

}

AtomicAgent::TypeMap AtomicAgent::toTypeMap(const AtomicAgent::Set& atomicAgents)
{
    TypeMap typeMap;
    for(const AtomicAgent& aa : atomicAgents)
    {
        typeMap[ aa.getModel() ].insert(aa);
    }
    return typeMap;
}

std::string AtomicAgent::toString(const TypeMap& typeMap, size_t indent)
{
    std::stringstream ss;
    std::string hspace(indent,' ');

    for(const TypeMap::value_type& v : typeMap)
    {
        ss << hspace << v.first.getFragment() << ":" << std::endl;
        ss << hspace << "    ";
        AtomicAgent::Set::iterator cit = v.second.begin();
        for(;cit != v.second.end();)
        {
            ss << cit->getId();
            if(++cit == v.second.end())
            {
                break;
            } else {
                ss << ", ";
            }
        }
        ss << std::endl;
    }
    return ss.str();
}

} // end namespace moreorg
