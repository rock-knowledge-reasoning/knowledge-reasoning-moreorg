#ifndef ORGANIZATION_MODEL_METRIC_HPP
#define ORGANIZATION_MODEL_METRIC_HPP

#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <organization_model/ModelPool.hpp>
#include <organization_model/OrganizationModel.hpp>

namespace organization_model {

// TODO: commment from ICRA Reviewer paper 2015
// how can further merit functions like information again, failure rates
// or similar be combined into the probability of survival metric

namespace metrics {

enum Type { UNKNOWN = 0, REDUNDANCY };

} // end namespace metrics

typedef owlapi::model::IRI ServiceIRI;
typedef owlapi::model::IRI ActorIRI;
typedef std::map< std::pair<ServiceIRI, ActorIRI>, double> MetricMap;

/**
 * Interface for simple metrics
 *
 * TODO: Add metrics that have time dependencies
 */
class Metric
{
public:

    /**
     * Metric for an organization model
     */
    Metric(const OrganizationModel& organization, metrics::Type type);

    virtual ~Metric() {}

    typedef boost::shared_ptr<Metric> Ptr;

    /**
     * Compute all metrics for all combinations of services and atomic actors
     * \return Map with computed metric
     */
    MetricMap getMetricMap() const;

    /**
     * Compute the metric for a given function (service) and an actor model
     * \return computed metric
     */
    virtual double compute(const owlapi::model::IRI& function, const owlapi::model::IRI& model) const { throw std::runtime_error("organization_model::metrics::Metric::compute(function,model) not implemented"); }

    /**
     * Compute the metric for a given function (service) and a model pool
     * describing a composite actor
     */
    virtual double compute(const owlapi::model::IRI& function, const ModelPool& modelPool) const { throw std::runtime_error("organization_model::metrics::Metric::compute(function,modelPool) not implemented"); }


    static std::string toString(const MetricMap& map, uint32_t indent = 0);

protected:
    OrganizationModel mOrganizationModel;
    boost::shared_ptr<owlapi::model::OWLOntologyAsk> mpAsk;

    metrics::Type mType;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_METRIC_HPP
