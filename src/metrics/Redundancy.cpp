#include "Redundancy.hpp"
#include <boost/foreach.hpp>
#include <math.h>
#include <vector>
#include <base/Logging.hpp>

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
    std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = required.begin();
    std::map<IRI, uint32_t> availableResources = OWLCardinalityRestriction::convertToExactMapping(available);

    // Firstly -- we need to find a propery match of required resources to
    // available resources, thus defining here a small Constraint Satisfaction
    // Problem already


    double probabilityOfSurvival = 1;
    for(; cit != required.end(); ++cit)
    {
        IRI qualification = (*cit)->getQualification();
        uint32_t cardinality = (*cit)->getCardinality();
/*
        IRIList mpOntology->ancestors(

        uint32_t available = availableResourceMap[ cit->first ];

        double redundancy = 0;
        if(available)
        {
            // available vs. required
            redundancy = available / (1.0 * cit->second);
            LOG_DEBUG_S << "Redundancy level for: " << cit->first << " is " << redundancy << std::endl
                << "    available: " << available << std::endl
                << "    required:  " << cit->second;
        } else {
            LOG_DEBUG_S << "No resource available for " << cit->first << "; probability of survival will be 0";
            return 0;
        }

        // Mean probability of failure
        // Probability of component failure
        // default is p=0.5 
        double pSum = 0;
        BOOST_FOREACH(const IRI& component, components)
        {
            try {
                // SCHOKO
                OWLLiteral::Ptr value = mAsk.getDataValue(component, OM::probabilityOfFailure());
                LOG_DEBUG_S << "Retrieved probability of failure for '" << component << ": " << value->getDouble();
                pSum += 1 - value->getDouble();
            } catch(...)
            {
                LOG_DEBUG_S << "Default probability of failure for '" << component << ": 0.5";
                pSum += 0.5;
            }
        }

        double p = pSum / (1.0*components.size());

        // Serial chain of parallel components thus multiplication of R(t)
        // Zuverlässigkeitstechnik, Meyna and Pauli, 2 Ed p.175
        probabilityOfSurvival *= 1 - pow(1 - p,redundancy);
    */
    }

    //return probabilityOfSurvival;
    return 0.0;
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


