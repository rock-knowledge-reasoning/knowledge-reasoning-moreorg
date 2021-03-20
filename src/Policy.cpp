#include "Policy.hpp"
#include "facades/Robot.hpp"

using namespace owlapi::model;

namespace moreorg {

std::map<owlapi::model::IRI, Policy::CreationFunc > Policy::msCreationFuncs;
std::map<owlapi::model::IRI, std::map<ModelPool, Policy::Ptr> > Policy::msPolicies;

Policy::Policy(const owlapi::model::IRI& iri)
    : mModelPool()
    , mAsk()
    , mIRI(iri)
{}

Policy::Policy(const ModelPool& pool,
        const OrganizationModelAsk& ask,
        const owlapi::model::IRI& iri)
    : mModelPool(pool)
    , mAsk(ask)
    , mIRI(iri)
{
}

Policy::~Policy()
{
}

std::map<owlapi::model::IRI, double> Policy::computeSharesByType(const
        owlapi::model::IRI& property,
        const ModelPool& modelPool,
        const OrganizationModelAsk& ask) const
{
    std::map<IRI, double> sharesByType;
    if(modelPool.empty())
    {
        throw
            std::invalid_argument("owlapi::model::Policy::computeSharesByType"
                    " invalid argument: model pool cannot be empty");
    } else if(modelPool.numberOfInstances() == 1)
    {
        sharesByType[ modelPool.begin()->first ] = 1.0;
    } else {
        double sumOfPropertyValues = 0;
        std::map<IRI, double> propertyValues;
        for(const ModelPool::value_type p :  modelPool)
        {
            const IRI& agentType = p.first;
            size_t numberOfInstances = p.second;

            facades::Robot robot = facades::Robot::getInstance(agentType, ask);

            double propertyValue = robot.getPropertyValue(property);
            propertyValues[agentType] = propertyValue;
            sumOfPropertyValues += propertyValue*numberOfInstances;
        }

        for(const std::pair<IRI,double>& p : propertyValues)
        {
            sharesByType[p.first] = p.second / sumOfPropertyValues;
        }
    }
    return sharesByType;
}

Policy::Ptr Policy::getInstance(const owlapi::model::IRI& policyName,
            const ModelPool& pool,
            const OrganizationModelAsk& ask)
{
    Policy::Ptr policy = msPolicies[policyName][pool];
    if(!policy)
    {
        std::map<IRI, CreationFunc>::const_iterator fit = msCreationFuncs.find(policyName);
        if(fit != msCreationFuncs.end())
        {
            const CreationFunc& cFunc = fit->second;
            policy = cFunc(pool, ask);
            msPolicies[policyName][pool] = policy;
        }
    }
    return policy;
}

void Policy::registerCreationFuncs(const owlapi::model::IRI& policyName,
        CreationFunc func)
{
    Policy::msCreationFuncs[policyName] = func;
}

} // end namespace moreorg
