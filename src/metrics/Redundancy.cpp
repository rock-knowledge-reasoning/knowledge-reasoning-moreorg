#include "Redundancy.hpp"
#include <math.h>
#include <vector>
#include <base/Logging.hpp>
#include <owlapi/csp/ResourceMatch.hpp>
#include <organization_model/metrics/ModelSurvivability.hpp>

using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace organization_model {
namespace metrics {

Redundancy::Redundancy(const OrganizationModel& organization)
    : Metric(organization, REDUNDANCY)
{}


double Redundancy::compute(const owlapi::model::IRI& function, const owlapi::model::IRI& model) const
{
    return computeModelBasedProbabilityOfSurvival(function, model);
}

double Redundancy::compute(const owlapi::model::IRI& function, const ModelPool& modelPool) const
{
    return computeModelBasedProbabilityOfSurvival(function, modelPool);
}

double Redundancy::computeModelBasedProbabilityOfSurvival(const IRI& function, const IRI& model) const
{
    // Get minimal requirements to maintain the function
    std::vector<OWLCardinalityRestriction::Ptr> requirements = mpAsk->getCardinalityRestrictions(function);
    // Get model restrictions, i.e. in effect what has to be available
    std::vector<OWLCardinalityRestriction::Ptr> availableResources = mpAsk->getCardinalityRestrictions(model);

    return compute(requirements, availableResources);
}

double Redundancy::computeModelBasedProbabilityOfSurvival(const owlapi::model::IRI& function, const ModelPool& modelPool) const
{
    // Get minimal requirements to maintain the function
    std::vector<OWLCardinalityRestriction::Ptr> requirements = mpAsk->getCardinalityRestrictions(function);

    // Get model restrictions, i.e. in effect what has to be available for the
    // given models
    ModelPool::const_iterator mit = modelPool.begin();
    std::vector<OWLCardinalityRestriction::Ptr> allAvailableResources;
    for(; mit != modelPool.end(); ++mit)
    {
        IRI model = mit->first;
        uint32_t modelCount = mit->second;

        std::vector<OWLCardinalityRestriction::Ptr> availableResources = mpAsk->getCardinalityRestrictions(model);
        std::vector<OWLCardinalityRestriction::Ptr>::iterator cit = availableResources.begin();
        for(; cit != availableResources.end(); ++cit)
        {
            OWLCardinalityRestriction::Ptr restriction = *cit;
            // Update the cardinality with the actual model count
            uint32_t cardinality = modelCount*restriction->getCardinality();
            restriction->setCardinality(cardinality);
        }

        allAvailableResources = owlapi::model::OWLCardinalityRestriction::join(allAvailableResources, availableResources);
    }
    return compute(requirements, allAvailableResources);
}

double Redundancy::compute(const std::vector<OWLCardinalityRestriction::Ptr>& required, const std::vector<OWLCardinalityRestriction::Ptr>& available) const
{
    using namespace owlapi::model;

    // Strategies to compute redundancy:
    // 1. account for relevant functionality only
    //
    // Best common redundancy:
    //  --- account for full replacements
    //   >> assign minimal resources
    //   >>   only successful full assignment, remove these resources and try
    //   again with the rest
    //
    //   >> check how often that can be done -- compute redundancy for serial
    //   system, allow parallel system being
    //
    // Best redundancy: assign resource where it contributes the most, i.e.
    //  --- system parts with lowest level of survivability gets resource first
    //  (since they will most likely fail, that should be fair enough)

    // Firstly -- we need to find a proper match of required resources to
    // available resources, thus defining here a small Constraint Satisfaction
    // Problem already
    using namespace owlapi::csp;

    ModelBound::List modelBoundRemaining = ResourceMatch::toModelBoundList(available);
    ModelBound::List modelBoundRequired = ResourceMatch::toModelBoundList(required);

    LOG_DEBUG_S << "Available: " << ModelBound::toString(modelBoundRemaining);
    LOG_DEBUG_S << "Required: " << ModelBound::toString(modelBoundRequired);

    ResourceMatch::Solution solution;
    uint32_t fullModelRedundancy = 0;
    try {

        // Check how often a full redundancy of the top level model is given
        while(true)
        {
            solution = ResourceMatch::solve(modelBoundRequired, modelBoundRemaining, mOrganizationModel.ontology());
            ++fullModelRedundancy;
            LOG_DEBUG_S << "Solution: " << solution.toString();
            // Remove the consumed models from the list of available and try to
            // repeat solving
            // throws invalid_argument when model bounds are exceeded
            modelBoundRemaining = solution.substractMinFrom(modelBoundRemaining);
            LOG_DEBUG_S << "Remaining: " << ModelBound::toString(modelBoundRemaining);
        }
    } catch(const std::runtime_error& e)
    {
        LOG_DEBUG_S << e.what();
    }

    LOG_INFO_S << "Full model redundancy count is at: " << fullModelRedundancy << std::endl
        << "   remaining: " << ModelBound::toString(modelBoundRemaining, 8);

    if(fullModelRedundancy == 0)
    {
        LOG_WARN_S << "Redundancy: the minimal resource requirements have not been fulfilled. Redundancy cannot be computed";
        throw std::runtime_error("owlapi::metrics::Redundancy: minimal resource requirement have not been fulfilled");
    }

    //  Find weakest spot in the model
    //  Put all resources in that match
    //  iterate until no resources are left

    std::vector<ModelSurvivability> models;

    std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = required.begin();
    for(; cit != required.end(); ++cit)
    {
        IRI qualification = (*cit)->getQualification();

        // Mean probability of failure
        // Probability of component failure
        // default is p=0.5
        double probabilityOfSurvival = 0;
        try {
            // SCHOKO: Model should have an associated probability of failure
            OWLLiteral::Ptr value = mpAsk->getDataValue(qualification, OM::probabilityOfFailure());
            LOG_DEBUG_S << "Retrieved probability of failure for '" << qualification << ": " << value->getDouble();
            probabilityOfSurvival = 1 - value->getDouble();
        } catch(...)
        {
            LOG_DEBUG_S << "Using probability of failure for '" << qualification << ": 0.5";
            probabilityOfSurvival = 0.5;
        }

        ModelSurvivability survivability(*cit, probabilityOfSurvival, fullModelRedundancy);
        models.push_back(survivability);
    }

    // Best model fit: redundancy
    bool updated = false;
    do
    {
        updated = false;
        // Sort based on probability of survival -- try to maximize redundancy
        std::sort(models.begin(), models.end(), [](const ModelSurvivability& a, const ModelSurvivability& b)
                {
                return a.getProbabilityOfSurvival() < b.getProbabilityOfSurvival();
                });

        ModelBound::List::iterator rit = modelBoundRemaining.begin();
        for(; rit != modelBoundRemaining.end();++rit)
        {
            ModelBound& remaining = *rit;

            // Try to fit remaining resources
            std::vector<ModelSurvivability>::iterator mit = models.begin();
            for(; mit != models.end(); ++mit)
            {
                // Check if model can be used to strengthen the survivability
                if( mit->getQualification() == remaining.model || mpAsk->isSubClassOf(remaining.model, mit->getQualification()) )
                {
                    try {
                        remaining.decrement();

                        // Increase redundancy
                        // (mit->getCardinality)
                        mit->increment();
                        updated = true;
                    } catch(...)
                    {
                        modelBoundRemaining.erase(rit);
                        --rit;
                        break;
                    }
                }
            }
        }
    } while(updated);

    // Serial model of all subcomponents --> the full system
    double fullModelSurvival = 1;

    std::vector<ModelSurvivability>::iterator mit = models.begin();
    for(; mit != models.end(); ++mit)
    {
        LOG_INFO_S << "Probability of survival: " << mit->toString();
        fullModelSurvival *= mit->getProbabilityOfSurvival();
    }

    return fullModelSurvival;
}

double Redundancy::parallel(const std::vector<double>& probabilities)
{
    double probability = 1.0;
    std::vector<double>::const_iterator cit = probabilities.begin();
    for(; cit != probabilities.end(); ++cit)
    {
        probability *= (1 - *cit);
    }

    return 1 - probability;

}

double Redundancy::serial(const std::vector<double>& probabilities)
{
    double probability = 1.0;
    std::vector<double>::const_iterator cit = probabilities.begin();
    for(; cit != probabilities.end(); ++cit)
    {
        probability *= *cit;
    }
    return probability;
}

} // end namespace metrics
} // end namespace organization_model


