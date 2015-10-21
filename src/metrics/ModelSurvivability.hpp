#ifndef ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP
#define ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP

#include <owlapi/OWLApi.hpp>

namespace organization_model {
namespace metrics {

/**
 * Allow to define the survivability for a given model
 */
class ModelSurvivability
{
public:
    ModelSurvivability(const owlapi::model::OWLCardinalityRestriction::Ptr& restriction, double modelProbabilityOfSurvival, double redundancy);

    /**
     * Get the probability of survival for this model
     * account for the overall redundancy and cardinality
     */
    double getProbabilityOfSurvival() const;

    /**
     * Get the qualification, i.e. the model name
     */
    owlapi::model::IRI getQualification() const;

    /**
     * Get the cardinality (min/max)
     */
    uint32_t getCardinality() const;

    // Remaining resource (count is 1) over required count
    void increment() { mRedundancy += 1.0/(1.0*getCardinality()); }

    std::string toString() const;

private:
    owlapi::model::OWLCardinalityRestriction::Ptr mRestriction;

    /// Probability of survival of this model
    double mModelProbabilityOfSurvival;
    /// Redundancy of this model
    double mRedundancy;

};

} // end namespace metrics
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP
