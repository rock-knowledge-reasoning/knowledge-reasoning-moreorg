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

uint32_t Redundancy::computeOutDegree(const IRI& iri, const IRI& relation, const IRI& klass)
{
    IRIList related = mAsk.allRelatedInstances(iri, relation, klass);
    return related.size();
}

uint32_t Redundancy::computeInDegree(const IRI& iri, const IRI& relation, const IRI& klass)
{
    IRIList related = mAsk.allInverseRelatedInstances(iri, relation, klass);
    return related.size();
}

double Redundancy::computeModelBasedProbabilityOfSurvival(const IRI& function, const IRI& model)
{
    // Get minimal requirements to maintain the function
    std::vector<OWLCardinalityRestriction::Ptr> requirements = mAsk.getCardinalityRestrictions(function);
    // Get model restrictions, i.e. in effect what has to be available
    std::vector<OWLCardinalityRestriction::Ptr> availableResources = mAsk.getCardinalityRestrictions(model);
    return 0.0;



//    // Get count for requirement type per resource type
//    IRI model = mOrganizationModel.getResourceModel( resource );
//    IRIList dependantResources = mAsk.allRelatedInstances( model, OM::dependsOn() );
//    std::map<IRI, uint32_t> resourceRequirementCount;
//    BOOST_FOREACH(const IRI& dependant, dependantResources)
//    {
//        IRI resourceModel = mOrganizationModel.getResourceModel( dependant );
//        uint32_t currentValue = resourceRequirementCount[ resourceModel ];
//        resourceRequirementCount[ resourceModel ] = currentValue + 1;
//    }
//
//    // Get count for available resources
//    IRIList availableResources = mAsk.allRelatedInstances( actor, OM::has() );
//    std::map<IRI, IRIList> availableResourceMap;
//    BOOST_FOREACH(const IRI& availableResource, availableResources)
//    {
//        IRI resourceModel = mOrganizationModel.getResourceModel( availableResource );
//        availableResourceMap[ resourceModel ].push_back( availableResource );
//    }
//
//    // The instance for the requested resource (model) which can be updated
//    availableResources = mAsk.allRelatedInstances( actor, OM::provides() );
//    BOOST_FOREACH(const IRI& availableResource, availableResources)
//    {
//        IRI resourceModel = mOrganizationModel.getResourceModel( availableResource );
//        availableResourceMap[ resourceModel ].push_back( availableResource );
//    }
//
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
    try {
        owlapi::csp::InstanceList resources = owlapi::csp::ResourceMatch::getInstanceList(available);
        // Check how often a full redundancy of the top level model is given
        while(true)
        {
            match = owlapi::csp::ResourceMatch::solve(required, resources, mOrganizationModel.ontology());
            resources = match->getUnassignedResources();
            fullModelRedundancy++;
        }
    } catch(const std::runtime_error& )
    {
        remainingResources = match->getUnassignedResources();
        LOG_WARN_S << "No solution found -- full model redundancy count is at: " << fullModelRedundancy << 
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
        uint32_t cardinality = (*cit)->getCardinality();
        
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
        LOG_WARN_S << "Probability of survival: " << mit->toString(); 
        fullModelSurvival *= mit->getProbabilityOfSurvival();
    }

    return fullModelSurvival;
}

IRIMetricMap Redundancy::compute()
{
    IRIMetricMap metricMap;
//    IRIList resources = mAsk.allInstancesOf( OM::Resource() );
//    IRI relation = OM::uses();
//
//    BOOST_FOREACH(const IRI& resource, resources)
//    {
//        uint32_t inDegree = computeInDegree(resource, relation);
//        uint32_t outDegree = computeOutDegree(resource, relation);
//
//        Metric metric(relation.toString(), outDegree, inDegree);
//        metricMap[resource] = metric;
//
//        LOG_DEBUG_S << "Metric: " << std::endl
//            << "    instance: " << resource << std::endl
//            << "    metric:   " << metric.toString();
//    }
//
//    IRIList services = mAsk.allInstancesOf( OM::ServiceModel() );
//    IRIList actors = mAsk.allInstancesOf( OM::Actor() );
//    LOG_DEBUG_S << "Compute probabilityOfSurvival for: " << services;
//
//    BOOST_FOREACH(const IRI& actor, actors)
//    {
//        BOOST_FOREACH(const IRI& service, services)
//        {
//            try {
//                double probabilityOfSurvival = computeModelBasedProbabilityOfSurvival(service ,actor);
//
//                IRI instance = mOrganizationModel.getRelatedProviderInstance(actor, service);
////                mOrganizationModel.setDouble(instance, OM::probabilityOfFailure(), 1 - probabilityOfSurvival);
//
//                LOG_DEBUG_S << "Probability of survival:" << std::endl
//                    << "    actor :  " << actor << std::endl
//                    << "    service: " << service << std::endl
//                    << "    value:   " << probabilityOfSurvival;
//            } catch(const std::invalid_argument& e)
//            {
//                LOG_DEBUG_S << "Redundancy: computing probability failed: " << e.what();
//            }
//        }
//    }

    return metricMap;
}

} // end namespace metrics
} // end namespace owl_om


