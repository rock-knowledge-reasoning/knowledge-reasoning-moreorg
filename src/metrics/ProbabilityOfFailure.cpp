#include "ProbabilityOfFailure.hpp"
#include <stdexcept>

namespace moreorg
{
    namespace metrics
    {

        ProbabilityOfFailure::ProbabilityOfFailure(const reasoning::ModelBound &requirement,
                                                   const ResourceInstance::List &assignments,
                                                   const ProbabilityDensityFunction::Ptr &resourcePoFDistribution)
            : Probability(requirement, assignments), mModelProbabilityOfFailureDistribution(resourcePoFDistribution)
        {
            if (!mModelProbabilityOfFailureDistribution)
            {
                throw std::invalid_argument("moreorg::metrics::ProbabilityOfFailure no model provided!");
            }
            mRedundancy = mAssignments.size() / getCardinality();
        }

        double ProbabilityOfFailure::getProbabilityOfFailure(double time) const
        {
            double pos = 1 - mModelProbabilityOfFailureDistribution->getValue(time);
            double pSerialSystem = pow(pos, getCardinality());
            return 1 - pSerialSystem;
        }

        double ProbabilityOfFailure::getProbabilityOfSurvival(double time) const
        {
            return 1 - getProbabilityOfFailure(time);
        }

        double ProbabilityOfFailure::getProbabilityOfFailureConditional(double time_start, double time_end) const
        {
            double pos = 1 - mModelProbabilityOfFailureDistribution->getConditional(time_start, time_end);
            double pSerialSystem = pow(pos, getCardinality());
            return 1 - pSerialSystem;
        }

        double ProbabilityOfFailure::getProbabilityOfSurvivalConditional(double time_start, double time_end) const
        {
            return 1 - getProbabilityOfFailureConditional(time_start, time_end);
        }

        double ProbabilityOfFailure::getProbabilityOfFailureWithRedundancy(double time) const
        {
            double pos = 1 - mModelProbabilityOfFailureDistribution->getValue(time);
            double pSerialSystem = pow(pos, getCardinality());
            return pow(1 - pSerialSystem, mRedundancy);
        }

        double ProbabilityOfFailure::getProbabilityOfSurvivalWithRedundancy(double time) const
        {
            return 1 - getProbabilityOfFailureWithRedundancy(time);
        }

        double ProbabilityOfFailure::getProbabilityOfFailureConditionalWithRedundancy(double time_start, double time_end) const
        {
            double pos = 1 - mModelProbabilityOfFailureDistribution->getConditional(time_start, time_end);
            double pSerialSystem = pow(pos, getCardinality());
            return pow(1 - pSerialSystem, mRedundancy);
        }

        double ProbabilityOfFailure::getProbabilityOfSurvivalConditionalWithRedundancy(double time_start, double time_end) const
        {
            return 1 - getProbabilityOfFailureConditionalWithRedundancy(time_start, time_end);
        }

        ProbabilityDensityFunction::Ptr ProbabilityOfFailure::getProbabilityDensityFunction() const
        {
            return mModelProbabilityOfFailureDistribution;
        }

        void ProbabilityOfFailure::addAssignment(ResourceInstance &assignment)
        {
            mAssignments.push_back(assignment);
            increment();
        }

        void ProbabilityOfFailure::removeAssignment(ResourceInstance &assignmentToRemove)
        {
            auto it = std::find_if(mAssignments.begin(), mAssignments.end(),
                                   [&assignmentToRemove](
                                       const ResourceInstance assignment)
                                   {
                                       return assignment.getName() == assignmentToRemove.getName();
                                   });
            if (it == mAssignments.end())
            {
                throw std::runtime_error("moreorg::metrics::ProbabilityOfFailure::removeAssignment could not find assignment and remove it.");
            }
            else
            {
                mAssignments.erase(it);
                decrement();
            }
        }

        std::string ProbabilityOfFailure::toString() const
        {
            std::stringstream ss;
            ss << " ProbabilityOfFailure: " << std::endl;
            ss << "    modelProbability:      " << mModelProbabilityOfFailureDistribution->getValue() << std::endl;
            ss << "    redundancy:            " << mRedundancy << std::endl;
            ss << "    probabilityOfFailure: " << getProbabilityOfFailureWithRedundancy() << std::endl;
            ss << "  > " << mRequirement.toString();
            return ss.str();
        }

    } // end namespace metrics
} // end namespace moreorg
