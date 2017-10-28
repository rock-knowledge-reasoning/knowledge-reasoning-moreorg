#include "Metric.hpp"
#include <sstream>
#include <base-logging/Logging.hpp>
#include "vocabularies/OM.hpp"
#include "metrics/Redundancy.hpp"

namespace organization_model {

std::map<metrics::Type,Metric::Ptr> Metric::msMetrics;

Metric::Metric(metrics::Type type, const OrganizationModelAsk& organization)
    : mOrganizationModelAsk(organization)
    , mType(type)
{}


MetricMap Metric::getMetricMap() const
{
    using namespace owlapi::model;
    using namespace owlapi::vocabulary;

    MetricMap metricMap;

    IRIList actorModels = mOrganizationModelAsk.ontology().allSubClassesOf( vocabulary::OM::Actor() );
    IRIList services = mOrganizationModelAsk.ontology().allSubClassesOf( vocabulary::OM::Service() );

    for(const IRI& actorModel : actorModels)
    {
        for(const IRI& service : services)
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

double Metric::compute(const owlapi::model::IRI& function, const owlapi::model::IRI& model) const
{
    ModelPool modelPool;
    modelPool[model] = 1;

    return compute(function, modelPool);
}

double Metric::compute(const owlapi::model::IRI& function, const ModelPool& modelPool) const
{
    using namespace owlapi::model;
    IRISet functionSet;
    functionSet.insert(function);

    return computeExclusiveUse(functionSet, modelPool);
}

double Metric::computeSharedUse(const ModelPool& required, const ModelPool& available) const
{
    using namespace owlapi::model;
    std::vector<OWLCardinalityRestriction::Ptr> r_available = mOrganizationModelAsk.getCardinalityRestrictions(available, OWLCardinalityRestriction::SUM_OP, false);
    std::vector<OWLCardinalityRestriction::Ptr> r_required = mOrganizationModelAsk.getCardinalityRestrictions(required, OWLCardinalityRestriction::MAX_OP, true);

    return computeMetric(r_required, r_available);

}

double Metric::computeSharedUse(const owlapi::model::IRISet& functions, const ModelPool& modelPool) const
{
    using namespace owlapi::model;

    std::vector<double> values;
    IRISet::const_iterator fit = functions.begin();
    for(; fit != functions.end(); ++fit)
    {
        const IRI& function = *fit;
        values.push_back( compute(function, modelPool) );
    }

    return sequentialUse(values);
}

double Metric::computeExclusiveUse(const ModelPool& required, const ModelPool& available) const
{
    using namespace owlapi::model;
    std::vector<OWLCardinalityRestriction::Ptr> r_available = mOrganizationModelAsk.getCardinalityRestrictions(available, OWLCardinalityRestriction::SUM_OP, false);
    std::vector<OWLCardinalityRestriction::Ptr> r_required = mOrganizationModelAsk.getCardinalityRestrictions(required, OWLCardinalityRestriction::SUM_OP, true);

    return computeMetric(r_required, r_available);

}

double Metric::computeExclusiveUse(const owlapi::model::IRISet& functions, const ModelPool& modelPool) const
{
    using namespace owlapi::model;

    std::vector<OWLCardinalityRestriction::Ptr> requirements;

    ModelPool required;
    IRISet::const_iterator fit = functions.begin();
    for(; fit != functions.end(); ++fit)
    {
        const IRI& function = *fit;
        required[function] = 1;
    }
    requirements = mOrganizationModelAsk.getCardinalityRestrictions(required, OWLCardinalityRestriction::SUM_OP);

    // Get model restrictions, i.e. in effect what has to be available for the
    // given models
    std::vector<OWLCardinalityRestriction::Ptr> allAvailableResources = mOrganizationModelAsk.getCardinalityRestrictions(modelPool, OWLCardinalityRestriction::SUM_OP);

    return computeMetric(requirements, allAvailableResources);
}


Metric::Ptr Metric::getInstance(metrics::Type type, const OrganizationModelAsk& organization)
{
    std::map<metrics::Type, Metric::Ptr>::const_iterator cit = msMetrics.find(type);
    if(cit != msMetrics.end())
    {
        return cit->second;
    }

    switch(type)
    {
        case metrics::REDUNDANCY:
        {
            Metric::Ptr metric(new metrics::Redundancy(organization));
            msMetrics[metrics::REDUNDANCY] = metric;
            return metric;
        }
        default:
            break;
    }
    throw std::invalid_argument("organization_model::Metric::getInstance: failed to retrieve a type");
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

