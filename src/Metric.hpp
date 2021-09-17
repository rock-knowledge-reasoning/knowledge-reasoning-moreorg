#ifndef ORGANIZATION_MODEL_METRIC_HPP
#define ORGANIZATION_MODEL_METRIC_HPP

#include <stdexcept>
#include <owlapi/model/OWLCardinalityRestriction.hpp>
#include "ModelPool.hpp"
#include "OrganizationModelAsk.hpp"
#include "vocabularies/OM.hpp"
#include "reasoning/ModelBound.hpp"
#include "ResourceInstance.hpp"
#include <random>

namespace moreorg {

// TODO: commment from ICRA Reviewer paper 2015
// how can further merit functions like information gain, failure rates
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
    typedef shared_ptr<Metric> Ptr;
    typedef std::vector<Metric> List;

    /**
     * Metric for an organization model
     */
    Metric(metrics::Type type,
            const OrganizationModelAsk& organization,
            const owlapi::model::IRI& property = vocabulary::OM::has());

    virtual ~Metric() = default;

    /**
     * Compute all metrics for all combinations of services and atomic actors
     * \return Map with computed metric
     */
    MetricMap getMetricMap() const;

    /**
     * Compute the metric for a given function (service) and a single agent model
     * \return computed metric
     */
    double compute(const owlapi::model::IRI& function, const ResourceInstance& model) const;

    /**
     * Compute the metric for a given function (service) and a agent defined by
     * a model pool, based on the exclusive resource usage
     */
    double compute(const owlapi::model::IRI& function, const ResourceInstance::List& modelPool) const;

    /**
     * Compute the exclusive use metric for two model pools
     * \param required Set of required resources
     * \param available Set of available resources
     * \see computeExclusiveUse
     */
    double computeExclusiveUse(const ModelPool& required, const ResourceInstance::List& available) const;

    /**
     * Compute the metric for a given set of functions (services)
     * \param functions Set of functions that require
     * the exclusive use of available resource, i.e., function requirements
     * add(!) up
     * \param modelPool The available set of resources
     * \return metric
     */
    double computeExclusiveUse(const owlapi::model::IRISet& functions, const ResourceInstance::List& modelPool) const;

    /**
     * Compute the shared use metric to compute a metric for required vs.
     * available resources
     * \param required Required resources
     * \param available Available resources
     */
    double computeSharedUse(const ModelPool& required, const ResourceInstance::List& available, double t0 = 0, double t1 = 0) const;

    /**
     * Compute the metric for a given set of functions (services)
     * \param functions Set of function that allow
     * a shared use of available resource, i.e., function requirements
     * do not add up, but are lower bounds
     * \param modelPool describing a composite actor
     * \return metric
     */
    double computeSharedUse(const owlapi::model::IRISet& functions, const ResourceInstance::List& modelPool) const;

    /**
     * Compute metric based on the given list of cardinality restrictions
     */
    virtual double computeMetric(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required, const ResourceInstance::List& available,
                                 double t0 = 0, double t1 = 0) const { throw std::runtime_error("moreorg::metrics::Metric::compute: not implemented"); }

    /**
     * Get list of Metrics to be able to check whether the corresponding component(s) survive or not or handle metric value calculations elsewhere
     */

    virtual Metric::List getSharedUseMetricModelsList(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required, std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& available,
                                 double t0 = 0, double t1 = 0) const { throw std::runtime_error("moreorg::metrics::Metric::compute: not implemented"); }

    /**
     * Compute the metric for a given list of single functions (services) that are
     * used sequentially and a model pool describing a composite actor that has
     * to provide this functions
     */
    virtual double computeSequential(const owlapi::model::IRIList& functions, const ModelPool& modelPool) const { throw std::runtime_error("moreorg::metrics::Metric::computeSequential(IRIList,ModelPool) not implemented"); }

    /**
     * Compute the metric for a sequential application of distinct function sets
     * by a composite actor which is described by the modelPool
     */
    virtual double computeSequential(const std::vector<owlapi::model::IRISet>& functionalRequirement, const ModelPool& modelPool, bool sharedUse = true) const { throw std::runtime_error("moreorg::metrics::Metric::compute(functions,modelPool) not implemented"); }

    /**
     * Compute a particular metric for a required model pool and an available
     * \return computed metric
     */
    static Metric::Ptr getInstance(metrics::Type type,
            const OrganizationModelAsk& organization);

    static std::string toString(const MetricMap& map, uint32_t indent = 0);

    /**
     * Compute the metrics for a given sequential chain of components
     */
    virtual double sequentialUse(const std::vector<double>& values) const { throw std::runtime_error("moreorg::metrics::Metric::sequentialUse  not implemented"); }

protected:
    OrganizationModelAsk mOrganizationModelAsk;
    metrics::Type mType;
    owlapi::model::IRI mProperty;
    static std::map<metrics::Type, Metric::Ptr> msMetrics;
    ResourceInstance::PtrList mResourceInstanceAssignments;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_METRIC_HPP
