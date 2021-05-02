#include "Policy.hpp"
#include "facades/Robot.hpp"
#include "policies/DistributionPolicy.hpp"
#include "policies/SelectionPolicy.hpp"
#include "policies/AgentSizeBasedSelection.hpp"
#include "policies/FunctionalityBasedSelection.hpp"
#include "policies/PropertyBasedSelection.hpp"

using namespace owlapi::model;

namespace moreorg {

std::map<owlapi::model::IRI, Policy::Ptr> Policy::msPolicies;

Policy::Policy(const owlapi::model::IRI& iri)
    : mIRI(iri)
{}

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
            const OrganizationModelAsk& ask)
{
    Policy::Ptr policy = msPolicies[policyName];
    if(policy)
    {
        return policy;
    }

    if(ask.ontology().isInstanceOf(policyName,
                vocabulary::OM::resolve("SelectionPolicy")))
    {
        policies::SelectionPolicy::Ptr selectionPolicy = make_shared<policies::SelectionPolicy>(policyName);
        for(size_t i = 0; i < MAX_POLICY_ELEMENTS; ++i)
        {
            std::stringstream ss;
            ss << "_" << i;
            IRI indexPlaceholder = vocabulary::OM::resolve(ss.str());
            IRIList instances = ask.ontology().allRelatedInstances(policyName, indexPlaceholder);
            if(instances.size() == 0)
            {
                break;
            } else if(instances.size() > 1)
            {
                throw std::runtime_error("moreorg::Policy::getInstance: index placeholder '"
                        + indexPlaceholder.toString() + "' used multiple times");
            }

            IRI policyElement = instances[0];

            if(policyElement == vocabulary::OM::resolve("AllSelection"))
            {
                // as neutral element we can ignore it here, since the selection
                // is not changed
            } else if(ask.ontology().isInstanceOf(policyElement, vocabulary::OM::resolve("FunctionalityBasedSelection")))
            {
                OWLAnnotationValue::Ptr selectByValue =
                    ask.ontology().getAnnotationValue(policyElement,
                            vocabulary::OM::resolve("selectBy"));

                IRI functionality = selectByValue->asIRI();

                policies::SelectionPolicy::Ptr fbs = make_shared<policies::FunctionalityBasedSelection>(functionality);
                selectionPolicy->add(fbs);
            } else if(ask.ontology().isInstanceOf(policyElement, vocabulary::OM::resolve("PropertyBasedSelection")))
            {
                OWLAnnotationValue::Ptr selectByValue =
                    ask.ontology().getAnnotationValue(policyElement,
                            vocabulary::OM::resolve("selectBy"));

                IRI property = selectByValue->asIRI();

                IRIList operators = ask.ontology().allRelatedInstances(policyElement,
                        vocabulary::OM::resolve("hasOperator"));

                IRI operatorName = operators[0];

                policies::SelectionPolicy::Ptr pbs = make_shared<policies::PropertyBasedSelection>(property, operatorName);
                selectionPolicy->add(pbs);
            } else if(ask.ontology().isInstanceOf(policyElement,
                        vocabulary::OM::resolve("AgentSizeBasedSelection")))
            {
                IRISet relations = ask.ontology().getRelatedDataProperties(policyElement);
                if(relations.size() != 1)
                {
                    throw std::runtime_error("moreorg::Policies::getInstance '"
                            + policyElement.toString() + "' misses comparison"
                            " operator");
                }

                const IRI& relation = *relations.begin();
                OWLLiteral::Ptr literal = ask.ontology().getDataValue(policyElement, relation, false);
                size_t value = literal->getDouble();

                policies::AgentSizeBasedSelection::Ptr abs =
                    make_shared<policies::AgentSizeBasedSelection>(relation,
                            value);
                selectionPolicy->add(abs);
            }
        }

        msPolicies[policyName] = selectionPolicy;
        return selectionPolicy;
    } else if(ask.ontology().isInstanceOf(policyName,
                vocabulary::OM::resolve("DistributionPolicy")))
    {

        policies::DistributionPolicy::Ptr distributionPolicy =
            make_shared<policies::DistributionPolicy>(policyName);

        for(size_t i = 0; i < MAX_POLICY_ELEMENTS; ++i)
        {
            std::stringstream ss;
            ss << "_" << i;
            IRI indexPlaceholder = vocabulary::OM::resolve(ss.str());
            IRIList instances = ask.ontology().allRelatedInstances(policyName, indexPlaceholder);
            if(instances.size() == 0)
            {
                break;
            } else if(instances.size() > 1)
            {
                throw std::runtime_error("moreorg::Policy::getInstance: index placeholder '"
                        + indexPlaceholder.toString() + "' used multiple times");
            }
            IRI policyElement = instances[0];

            // TODO

        }

    }

    throw std::runtime_error("moreorg::Policy::getInstance: could not identify"
            " policy for '" + policyName.toString() + "'");
}

} // end namespace moreorg
