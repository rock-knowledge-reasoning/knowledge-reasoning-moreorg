#include "Redundancy.hpp"
#include <boost/foreach.hpp>
#include <math.h>
#include <vector>
#include <base/Logging.hpp>
#include <owl_om/owlapi/csp/ResourceMatch.hpp>
#include <owl_om/metrics/ModelSurvivability.hpp>

using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace owl_om {
namespace metrics {

Redundancy::Redundancy(const OrganizationModel& organization)
    : mOrganizationModel(organization)
    , mAsk(mOrganizationModel.ontology())
{}

double Redundancy::computeModelBasedProbabilityOfSurvival(const IRI& function, const IRI& model)
{
    // Get minimal requirements to maintain the function
    std::vector<OWLCardinalityRestriction::Ptr> requirements = mAsk.getCardinalityRestrictions(function);
    // Get model restrictions, i.e. in effect what has to be available
    std::vector<OWLCardinalityRestriction::Ptr> availableResources = mAsk.getCardinalityRestrictions(model);

    return compute(requirements, availableResources);
}

double Redundancy::computeModelBasedProbabilityOfSurvival(const owlapi::model::IRI& function, const std::map<owlapi::model::IRI,uint32_t>& models)
{
    // Get minimal requirements to maintain the function
    std::vector<OWLCardinalityRestriction::Ptr> requirements = mAsk.getCardinalityRestrictions(function);

    // Get model restrictions, i.e. in effect what has to be available
    std::map<IRI, uint32_t>::const_iterator mit = models.begin();
    std::vector<OWLCardinalityRestriction::Ptr> allAvailableResources;
    for(; mit != models.end(); ++mit)
    {
        IRI model = mit->first;
        uint32_t count = mit->second;

        std::vector<OWLCardinalityRestriction::Ptr> availableResources = mAsk.getCardinalityRestrictions(model);
        std::vector<OWLCardinalityRestriction::Ptr>::iterator cit = availableResources.begin();
        for(; cit != availableResources.end(); ++cit)
        {
            OWLCardinalityRestriction::Ptr restriction = *cit;
            uint32_t cardinality = count*restriction->getCardinality();
            restriction->setCardinality(cardinality);
        }

        allAvailableResources.insert(allAvailableResources.end(), availableResources.begin(), availableResources.end());
    }
    return compute(requirements, allAvailableResources);
}

double Redundancy::compute(const std::vector<OWLCardinalityRestriction::Ptr>& required, const std::vector<OWLCardinalityRestriction::Ptr>& available)
{
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
    owlapi::csp::ResourceMatch* match = NULL;
    uint32_t fullModelRedundancy = 0;
    owlapi::csp::InstanceList remainingResources;
    owlapi::csp::InstanceList resources = owlapi::csp::ResourceMatch::getInstanceList(available);

    try {
        // Check how often a full redundancy of the top level model is given
        while(true)
        {
            match = owlapi::csp::ResourceMatch::solve(required, resources, mOrganizationModel.ontology());
            resources = match->getUnassignedResources();
            fullModelRedundancy++;
        }
    } catch(const std::runtime_error& )
    {
        if(match)
        {
            // the matching was successful at least once
            remainingResources = match->getUnassignedResources();
        } else {
            // the matching was never successful, this all available resources
            // remain
            remainingResources = resources;
        }
        LOG_INFO_S << "No solution found -- full model redundancy count is at: " << fullModelRedundancy <<
            " remainingResources " <<  remainingResources;
    }

    if(fullModelRedundancy == 0)
    {
        LOG_WARN_S << "Redundancy: the minimal resource requirements have not been fulfilled. Redundancy cannot be computed";
        throw std::runtime_error("owlapi::metrics::Redundancy: minimal resource requirement have not been fulfilled");
    }

    //  Find weakest spot in the model
    //  Put all resources in that match
    //  iterate until no resources are left

    std::vector<ModelSurvivability> models;

    // Compute full model redundancy and assuming serial connection
    // Serial chain of parallel components thus multiplication of R(t)
    // Zuverlässigkeitstechnik, Meyna and Pauli, 2 Ed p.275ff
    //
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
            OWLLiteral::Ptr value = mAsk.getDataValue(qualification, OM::probabilityOfFailure());
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
    while(true)
    {
        // Sort based on probability of survival -- try to maximize redundancy
        std::sort(models.begin(), models.end(), [](const ModelSurvivability& a, const ModelSurvivability& b)
                {
                return a.getProbabilityOfSurvival() < b.getProbabilityOfSurvival();
                });

        bool nextIteration = false;
        owlapi::csp::InstanceList::iterator rit = remainingResources.begin();
        for(; rit != remainingResources.end() && !nextIteration; ++rit)
        {
            // Try to fit remaining resources
            std::vector<ModelSurvivability>::iterator mit = models.begin();
            for(; mit != models.end(); ++mit)
            {
                if( mit->getQualification() == *rit || mAsk.isSubclassOf(*rit, mit->getQualification()) )
                {
                    // Increase redundancy
                    // Remaining resource (count is 1) over required count
                    // (mit->getCardinality)
                    mit->redundancy += 1.0/(1.0*mit->getCardinality());
                    remainingResources.erase(rit);
                    nextIteration = true;
                    break;
                }
            }
        }

        if(!nextIteration)
        {
            // no update so we can stop
            break;
        }
    }

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

IRISurvivabilityMap Redundancy::compute()
{
    IRISurvivabilityMap survivabilityMap;
    IRIList actorModels = mAsk.allSubclassesOf( OM::Actor() );

    IRIList services = mAsk.allSubclassesOf( OM::Service() );

    BOOST_FOREACH(const IRI& actorModel, actorModels)
    {
        BOOST_FOREACH(const IRI& service, services)
        {
            try {
                LOG_DEBUG_S << "Compute probabilityOfSurvival for: " << actorModel << " and " << service;

                double probabilityOfSurvival = computeModelBasedProbabilityOfSurvival(service, actorModel);
                std::pair<ServiceIRI, ActorIRI> key(service, actorModel);
                survivabilityMap[key] = probabilityOfSurvival;

                LOG_INFO_S << "Probability of survival:" << std::endl
                    << "    actorModel :  " << actorModel << std::endl
                    << "    service: " << service << std::endl
                    << "    value:   " << probabilityOfSurvival;

            } catch(const std::invalid_argument& e)
            {
                LOG_DEBUG_S << "Redundancy: computing probability failed: " << e.what();
            } catch(const std::runtime_error& e)
            {
                LOG_DEBUG_S << e.what();
                continue;
            }
        }
    }

    return survivabilityMap;
}

std::string Redundancy::toString(const IRISurvivabilityMap& map)
{
    std::stringstream ss;
    IRISurvivabilityMap::const_iterator cit = map.begin();
    for(; cit != map.end(); ++cit)
    {
        std::pair<ServiceIRI, ActorIRI> pair = cit->first;
        double survivability = cit->second;

        ss << "Service: " << pair.first << ", Actor: " << pair.second << " --> " << survivability;
        ss << std::endl;
    }

    return ss.str();
}

} // end namespace metrics
} // end namespace owl_om


