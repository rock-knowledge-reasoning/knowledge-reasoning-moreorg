#ifndef ORGANIZATION_MODEL_METRIC_HPP
#define ORGANIZATION_MODEL_METRIC_HPP

#include <stdexcept>
#include <organization_model/ModelPool.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <owlapi/model/OWLCardinalityRestriction.hpp>

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

    typedef shared_ptr<Metric> Ptr;

    /**
     * Compute all metrics for all combinations of services and atomic actors
     * \return Map with computed metric
     */
    MetricMap getMetricMap() const;

    /**
     * Compute the metric for a given function (service) and an actor model
     * \return computed metric
     */
    double compute(const owlapi::model::IRI& function, const owlapi::model::IRI& model) const;
    double compute(const owlapi::model::IRI& function, const ModelPool& modelPool) const;
    /**
     * Compute the metric for a given set of functions (services)
     * \param functions Set of function that require
     * the exclusive use of available resource, i.e., function requirements
     * add(!) up
     * \param modelPool describing a composite actor
     * \return metric
     */
    double computeExclusiveUse(const owlapi::model::IRISet& functions, const ModelPool& modelPool) const;

    /**
     * Compute the metric for a given set of functions (services)
     * \param functions Set of function that allow
     * a shared use of available resource, i.e., function requirements
     * do not add up, but are lower bounds
     * \param modelPool describing a composite actor
     * \return metric
     */
    double computeSharedUse(const owlapi::model::IRISet& functions, const ModelPool& modelPool) const;

    virtual double computeMetric(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required, const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& available) const { throw std::runtime_error("organization_model::metrics::Metric::compute: not implemented"); }

    /**
     * Compute the metric for a given list of single functions (services) that are
     * used sequentially and a model pool describing a composite actor that has
     * to provide this functions
     */
    virtual double computeSequential(const owlapi::model::IRIList& functions, const ModelPool& modelPool) const { throw std::runtime_error("organization_model::metrics::Metric::computeSequential(IRIList,ModelPool) not implemented"); }

    /**
     * Compute the metric for a sequential application of distinct function sets
     * by a composite actor which is described by the modelPool
     */
    virtual double computeSequential(const std::vector<owlapi::model::IRISet>& functionalRequirement, const ModelPool& modelPool, bool sharedUse = true) const { throw std::runtime_error("organization_model::metrics::Metric::compute(functions,modelPool) not implemented"); }

    static std::string toString(const MetricMap& map, uint32_t indent = 0);

    virtual double sequentialUse(const std::vector<double>& values) const { throw std::runtime_error("organization_model::metrics::Metric::sequentialUse  not implemented"); }

protected:
    OrganizationModel mOrganizationModel;
    shared_ptr<owlapi::model::OWLOntologyAsk> mpAsk;

    metrics::Type mType;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_METRIC_HPP
