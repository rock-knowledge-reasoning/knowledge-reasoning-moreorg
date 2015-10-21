#include "Metric.hpp"
#include <sstream>
#include <boost/foreach.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <base/Logging.hpp>

namespace organization_model {

Metric::Metric(const OrganizationModel& organization, metrics::Type type)
    : mOrganizationModel(organization)
    , mpAsk(new owlapi::model::OWLOntologyAsk(mOrganizationModel.ontology()))
    , mType(type)
{}

MetricMap Metric::getMetricMap() const
{
    using namespace owlapi::model;
    using namespace owlapi::vocabulary;

    MetricMap metricMap;

    IRIList actorModels = mpAsk->allSubClassesOf( OM::Actor() );
    IRIList services = mpAsk->allSubClassesOf( OM::Service() );

    BOOST_FOREACH(const IRI& actorModel, actorModels)
    {
        BOOST_FOREACH(const IRI& service, services)
        {
            try {
                LOG_DEBUG_S << "Compute metric for: " << actorModel << " and " << service;

                double metric = compute(service, actorModel);
                std::pair<ServiceIRI, ActorIRI> key(service, actorModel);
                metricMap[key] = metric;

                LOG_INFO_S << "Metric:" << std::endl
                    << "    actorModel :  " << actorModel << std::endl
                    << "    service: " << service << std::endl
                    << "    value:   " << metric;

            } catch(const std::invalid_argument& e)
            {
                LOG_WARN_S << "Metric: computing metric failed: " << e.what();
            } catch(const std::runtime_error& e)
            {
                LOG_DEBUG_S << e.what();
                continue;
            }
        }
    }

    return metricMap;
}

std::string Metric::toString(const MetricMap& map, uint32_t indent)
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    MetricMap::const_iterator cit = map.begin();
    for(; cit != map.end(); ++cit)
    {
        std::pair<ServiceIRI, ActorIRI> pair = cit->first;
        double metric = cit->second;

        ss << hspace << "Service: " << pair.first << ", Actor: " << pair.second << " --> metric: " << metric;
        ss << std::endl;
    }

    return ss.str();
}

} // end namespace organization_model

