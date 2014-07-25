#include "Redundancy.hpp"
#include <boost/foreach.hpp>

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
            << "    instance: " << resource
            << "    metric:   " << metric.toString();
    }

    return metricMap;
}

} // end namespace metrics
} // end namespace owl_om


