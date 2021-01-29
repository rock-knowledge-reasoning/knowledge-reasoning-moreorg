#ifndef ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP
#define ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP

#include <owlapi/OWLApi.hpp>

namespace moreorg {
namespace metrics {

/**
 * \class ModelSurvivability
 * Allow to define the survivability for a system model R_m - where the system
 * model is composed of uniform parts, based on the restriction r, which
 * defines the cardinality for the serial component, and the overall redundancy
 * of the mode which is computed from the available number of resources n vs.
 * the required r.
 \F[
     R_m = \begin{cases}
        1 - (1 - p_{r})^{\frac{n}{r}} & \text{, where } n \geq r \\
        0 & otherwise
    \end{cases}
 \F]
 *
 */
class ModelSurvivability
{
public:
    /**
     * Constructor for the ModelSurvivability of a single resource type
     * \param restriction Defines the requirement for a single resource type
     * The cardinality of the restriction gives the number of components which have to be in serial
     * for this model to work
     * e.g. stereo camera: 2 cameras
     * \param resourceProbabilityOfSurvival the probability of survival of the
     * resource type that builds the model
     * \param redundancy redundancy of the model, where redundancy provides the
     * information about the overall redundancy, e.g., if 4 cameras are
     * available, we assume a redundancy of 2
     */
    ModelSurvivability(
            const owlapi::model::OWLObjectCardinalityRestriction::Ptr& restriction,
            double resourceProbabilityOfSurvival,
            double redundancy
    );

    /**
     * Get the probability of survival for this model
     * account for the overall redundancy and cardinality
     * \return probability of survival
     */
    double getProbabilityOfSurvival() const;

    /**
     * Get the qualification, i.e. the model name
     * \return qualification
     */
    owlapi::model::IRI getQualification() const;

    /**
     * Get the cardinality (min/max) which defines the requirements for the
     * system model
     * \return cardinality of the restriction
     */
    uint32_t getCardinality() const;

    /**
     * Increment the redundancy based on addition a single resource of the same
     * type
     *
     */
    void increment() { mRedundancy += 1.0/(1.0*getCardinality()); }

    /**
     * Stringify object
     * \return string representation
     */
    std::string toString() const;

private:
    owlapi::model::OWLObjectCardinalityRestriction::Ptr mObjectRestriction;

    /// Probability of survival of this model
    double mModelProbabilityOfSurvival;
    /// Redundancy of this model
    double mRedundancy;
};

} // end namespace metrics
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_METRICS_MODEL_SURVIVABILITY_HPP
