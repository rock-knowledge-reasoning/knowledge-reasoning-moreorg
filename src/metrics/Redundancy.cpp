#include "Redundancy.hpp"
#include <boost/foreach.hpp>
#include <math.h>

namespace owl_om {
namespace metrics {

Redundancy::Redundancy(const OrganizationModel& organization)
    : mOrganizationModel(organization)
{}

uint32_t Redundancy::computeOutDegree(const IRI& iri, const IRI& relation, const IRI& klass)
{
    IRIList related = mOrganizationModel.ontology()->allRelatedInstances(iri, relation, klass);
    return related.size();
}

uint32_t Redundancy::computeInDegree(const IRI& iri, const IRI& relation, const IRI& klass)
{
    IRIList related = mOrganizationModel.ontology()->allInverseRelatedInstances(iri, relation, klass);
    return related.size();
}

double Redundancy::computeProbabilityOfSurvival(const IRI& resource, const IRI& actor)
{
    using namespace owl_om::vocabulary;

    // Get count for requirement type per resource type
    IRI model = mOrganizationModel.getResourceModel( resource );
    IRIList dependantResources = mOrganizationModel.ontology()->allRelatedInstances( model, OM::dependsOn() );
    std::map<IRI, uint32_t> resourceRequirementCount;
    BOOST_FOREACH(const IRI& dependant, dependantResources)
    {
        IRI resourceModel = mOrganizationModel.getResourceModel( dependant );
        uint32_t currentValue = resourceRequirementCount[ resourceModel ];
        resourceRequirementCount[ resourceModel ] = currentValue + 1;
    }

    // Get count for available resources
    IRIList availableResources = mOrganizationModel.ontology()->allRelatedInstances( actor, OM::has() );
    std::map<IRI, IRIList> availableResourceMap;
    BOOST_FOREACH(const IRI& availableResource, availableResources)
    {
        IRI resourceModel = mOrganizationModel.getResourceModel( availableResource );
        availableResourceMap[ resourceModel ].push_back( availableResource );
    }

    std::map<IRI, uint32_t>::const_iterator cit = resourceRequirementCount.begin();
    double probabilityOfFailure = 1;
    for(; cit != resourceRequirementCount.end(); ++cit)
    {
        IRIList components = availableResourceMap[ cit->first ];
        // Parallelity of components
        uint32_t available = components.size();
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
                DataValue value = mOrganizationModel.ontology()->getDataValue(component, OM::probabilityOfFailure());
                pSum += value.toDouble();
            } catch(...)
            {
                pSum += 0.5;
            }
        }

        double p = pSum / (1.0*components.size());

        // Serial chain of parallel components
        probabilityOfFailure *= pow(p,redundancy);
    }

    return 1 - probabilityOfFailure;
}

IRIMetricMap Redundancy::compute()
{
    using namespace owl_om::vocabulary;

    IRIMetricMap metricMap;
    IRIList resources = mOrganizationModel.ontology()->allInstancesOf( OM::Resource() );
    IRI relation = OM::uses();

    BOOST_FOREACH(const IRI& resource, resources)
    {
        uint32_t inDegree = computeInDegree(resource, relation);
        uint32_t outDegree = computeOutDegree(resource, relation);

        Metric metric(relation.toString(), outDegree, inDegree);
        metricMap[resource] = metric;

        LOG_DEBUG_S << "Metric: " << std::endl
            << "    instance: " << resource << std::endl
            << "    metric:   " << metric.toString();
    }

    IRIList services = mOrganizationModel.ontology()->allInstancesOf( OM::Service() );
    IRIList actors = mOrganizationModel.ontology()->allInstancesOf( OM::Actor() );

    BOOST_FOREACH(const IRI& actor, actors)
    {
        BOOST_FOREACH(const IRI& service, services)
        {
            double probabilityOfSurvival = computeProbabilityOfSurvival(service ,actor);

            LOG_DEBUG_S << "Probability of survival:" << std::endl
                << "    actor :  " << actor << std::endl
                << "    service: " << service << std::endl
                << "    value:   " << probabilityOfSurvival;
        }
    }

    return metricMap;
}

} // end namespace metrics
} // end namespace owl_om


