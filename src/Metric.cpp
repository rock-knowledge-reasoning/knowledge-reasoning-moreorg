#include "Metric.hpp"
#include <sstream>
#include <boost/foreach.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include <base-logging/Logging.hpp>

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

    IRIList actorModels = mpAsk->allSubClassesOf( vocabulary::OM::Actor() );
    IRIList services = mpAsk->allSubClassesOf( vocabulary::OM::Service() );

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

double Metric::computeExclusiveUse(const owlapi::model::IRISet& functions, const ModelPool& modelPool) const
{
    using namespace owlapi::model;

    std::vector<OWLCardinalityRestriction::Ptr> requirements;
    IRISet::const_iterator fit = functions.begin();
    for(; fit != functions.end(); ++fit)
    {
        const IRI& function = *fit;
        // Get minimal requirements to maintain the function
        std::vector<OWLCardinalityRestriction::Ptr> functionRequirements = mpAsk->getCardinalityRestrictions(function);
        requirements.insert(requirements.end(), functionRequirements.begin(), functionRequirements.end());
    }
    requirements = OWLCardinalityRestriction::compact(requirements);

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

    return computeMetric(requirements, allAvailableResources);
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

