#ifndef ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP
#define ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP

#include <organization_model/OrganizationModel.hpp>
#include <owlapi/model/OWLCardinalityRestriction.hpp>
#include <organization_model/Metric.hpp>

namespace organization_model {
namespace metrics {


/**
 * Compute redundancy for a model
 * \details Compute full model redundancy and assuming serial connection
 *  Serial chain of parallel components thus multiplication of R(t)
 *  Zuverlässigkeitstechnik, Meyna and Pauli, 2 Ed p.275ff
 *  Probability of survival of a system i: \f$p_i(t)\f$
 *  Survivability of a serial system:
 \f[
    R(t) = \Pi_{i=1}^{n} p_i(t)
 \f]
 *
 *  Parallel system:
 \f[
    R(t) = 1 - \Pi_{i=1}^{n} \left( 1-p_i(t) \right)
 \f]
 */
class Redundancy : public Metric
{
public:
    Redundancy(const OrganizationModel& organization);

    virtual double compute(const owlapi::model::IRI& function, const owlapi::model::IRI& model) const;

    virtual double compute(const owlapi::model::IRI& function, const ModelPool& modelPool) const;

    /**
     * Compute the probability of survival for a service/capability with respect to a given model
     *
     * It use the assigned restriction on that model to infer what is available
     * and matches this to the service requirements accordingly
     *
     */
    double computeModelBasedProbabilityOfSurvival(const owlapi::model::IRI& function, const owlapi::model::IRI& model) const;

    /**
     * Compute the probability of survival for the function of the system with
     * respect to a given set of composite actor model -- which is defined by a set of atomic
     * actor models
     */
    double computeModelBasedProbabilityOfSurvival(const owlapi::model::IRI& function, const ModelPool& models) const;


    double compute(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required,
            const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& available) const;

    /**
     * Compute survivability a parallel system
     * \param probabilities vector of probability of survival values
     */
    static double parallel(const std::vector<double>& probabilities);

    /**
     * Compute survivability of a serial system 
     * \param probabilities vector of probability of survival values
     */
    static double serial(const std::vector<double>& probabilities);
};

} // end namespace metrics
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP
