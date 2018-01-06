#include "AtomicAgent.hpp"
#include <sstream>

namespace organization_model {

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

organization_model::ModelPool AtomicAgent::getModelPool(const AtomicAgent::List& agents)
{
    organization_model::ModelPool modelPool;
    for(const AtomicAgent& agent : agents)
    {
        owlapi::model::IRI model = agent.getModel();
        modelPool[model] +=1;
    }
    return modelPool;
}

organization_model::ModelPool AtomicAgent::getModelPool(const AtomicAgent::Set& agents)
{
    organization_model::ModelPool modelPool;
    for(const AtomicAgent& agent : agents)
    {
        owlapi::model::IRI model = agent.getModel();
        modelPool[model] +=1;
    }
    return modelPool;
}

bool AtomicAgent::operator<(const AtomicAgent& other) const
{
    if(mModel == other.mModel)
    {
        return mId < other.mId;
    }
    return mModel < other.mModel;
}

AtomicAgent::List AtomicAgent::createAtomicAgents(const organization_model::ModelPool& modelPool)
{
    List agents;
    organization_model::ModelPool::const_iterator cit = modelPool.cbegin();
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

facets::Robot AtomicAgent::getFacet(const OrganizationModelAsk& ask) const
{
    return facets::Robot::getInstance(mModel, ask);

}

} // end namespace organization_model
