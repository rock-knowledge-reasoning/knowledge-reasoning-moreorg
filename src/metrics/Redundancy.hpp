#ifndef ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP
#define ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP

#include <organization_model/OrganizationModel.hpp>
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
    /**
     * Default probability of survival
     */
    Redundancy(const OrganizationModelAsk& organization, double defaultPoS = 0.95);

    double computeMetric(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required,
            const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& available) const;

    double computeSequential(const owlapi::model::IRIList& functions, const ModelPool& modelPool) const;
    double computeSequential(const std::vector<owlapi::model::IRISet>& functionalRequirement, const ModelPool& modelPool, bool sharedUse = true) const;

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

    /**
     * Compute metric interpretation when individual <function,model> tuples are
     * used sequentially
     */
    double sequentialUse(const std::vector<double>& values) const { return serial(values); }

    /**
     * Set the default probability survial that shall be used for computed the
     * redundancy
     */
    void setDefaultProbabilityOfSurvival(double p) { mDefaultProbabilityOfSurvival = p; }

    /**
     * Get the default probability survial that shall be used for computed the
     * redundancy
     */
    double getDefaultProbabilityOfSurvival() { return mDefaultProbabilityOfSurvival; }

private:
    double mDefaultProbabilityOfSurvival;
};

} // end namespace metrics
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP
