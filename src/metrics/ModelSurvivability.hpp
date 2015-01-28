#ifndef ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP
#define ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP

#include <owlapi/OWLApi.hpp>

namespace organization_model {
namespace metrics {

/**
 * Allow to define the survivability for a given model
 */
struct ModelSurvivability
{
    owlapi::model::OWLCardinalityRestriction::Ptr restriction;

    /// Probability of survival of this model
    double modelProbability;
    /// Redundancy of this model
    double redundancy;

    ModelSurvivability(owlapi::model::OWLCardinalityRestriction::Ptr restriction, double modelProbability, double redundancy);

    double getProbabilityOfSurvival() const;

    owlapi::model::IRI getQualification() const;
    uint32_t getCardinality() const;

    std::string toString() const;
};

} // end namespace metrics
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP
