#ifndef ORGANIZATION_MODEL_METRICS_PROBABILITY_OF_FAILURE_HPP
#define ORGANIZATION_MODEL_METRICS_PROBABILITY_OF_FAILURE_HPP

#include <owlapi/OWLApi.hpp>
#include <owlapi/model/IRI.hpp>
#include <random>
#include <cmath>
#include "DistributionFunctions.hpp"
#include "../ResourceInstance.hpp"
#include "../reasoning/ModelBound.hpp"
#include "Probability.hpp"

namespace moreorg
{
    namespace metrics
    {

        /**
         * \class ProbabilityOfFailure
         * Allow to define the survivability for a system model R_m - where the system
         * model is composed of uniform parts, based on the restriction r, which
         * defines the cardinality for the serial component, and the overall redundancy
         * of the mode which is computed from the available number of resources n vs.
         * the required r.
         
        *
        */
        class ProbabilityOfFailure : public Probability
        {
        public:
            /**
             * Constructor for the ProbabilityOfFailure of a single resource type
             * \param restriction Defines the requirement for a single resource type
             * The cardinality of the restriction gives the number of components which have to be in serial
             * for this model to work
             * e.g. stereo camera: 2 cameras
             * \param resourcePoFDistribution Probability of Failure Distribution Function used for this resource
             * \param redundancy redundancy of the model, where redundancy provides the
             * information about the overall redundancy, e.g., if 4 cameras are
             * available, we assume a redundancy of 2
             */
            ProbabilityOfFailure(
                const reasoning::ModelBound &requirement,
                const ResourceInstance::List &assignments,
                const ProbabilityDensityFunction::Ptr &resourcePoFDistribution);

            /**
             * Get the probability of Failure for this model
             * account for the overall redundancy and cardinality
             * \return probability of failure
             */
            double getProbabilityOfFailure(double time = 0) const;
            double getProbabilityOfSurvival(double time = 0) const;
            double getProbabilityOfFailureConditional(double time_start, double time_end) const;
            double getProbabilityOfSurvivalConditional(double time_start, double time_end) const;
            double getProbabilityOfFailureWithRedundancy(double time = 0) const;
            double getProbabilityOfSurvivalWithRedundancy(double time = 0) const;
            double getProbabilityOfFailureConditionalWithRedundancy(double time_start, double time_end) const;
            double getProbabilityOfSurvivalConditionalWithRedundancy(double time_start, double time_end) const;

            /**
            * Get the cardinality (min/max) which defines the requirements for the
            * system model
            * \return cardinality of the restriction
            */
            uint32_t getCardinality() const {return mRequirement.min;}

            ProbabilityDensityFunction::Ptr getProbabilityDensityFunction() const;

            /**
             * Increment the redundancy based on addition a single resource of the same
             * type
             *
             */

            void increment() { mRedundancy += 1.0 / (1.0 * getCardinality()); }

            /**
             * Increment the redundancy based on addition a single resource of the same
             * type
             *
             */
            void decrement() { mRedundancy -= 1.0 / (1.0 * getCardinality()); }

            void addAssignment(ResourceInstance &assignment);
            void removeAssignment(ResourceInstance &assignmentToRemove);

            /**
             * Stringify object
             * \return string representation
             */
            std::string toString() const;

        private:
            /// Probability of Failure of this model
            ProbabilityDensityFunction::Ptr mModelProbabilityOfFailureDistribution;
            /// Redundancy of this model
            double mRedundancy;
        };

    } // end namespace metrics
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_METRICS_PROBABILITY_OF_FAILURE_HPP
