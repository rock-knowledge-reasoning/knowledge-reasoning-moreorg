#include "Probability.hpp"


namespace moreorg {
namespace metrics {

    Probability::Probability(const reasoning::ModelBound &requirement, const ResourceInstance::List &assignments, const ProbabilityDensityFunction::Ptr &probabilityDensityFunction)
                            : mRequirement(requirement), mAssignments(assignments), mModelProbabilityDistribution(probabilityDensityFunction)
    {
    }

    Probability::Probability(const Probability &probability)
        {
             mModelProbabilityDistribution = probability.getProbabilityDensityFunction();
             mAssignments = probability.getAssignments();
             mRequirement = probability.getRequirement();
        }

    void Probability::removeAssignment(ResourceInstance &assignmentToRemove)
        {
            auto it = std::find_if(mAssignments.begin(), mAssignments.end(),
                                   [&assignmentToRemove](
                                       const ResourceInstance assignment)
                                   {
                                       return assignment.getName() == assignmentToRemove.getName();
                                   });
            if (it == mAssignments.end())
            {
                throw std::runtime_error("moreorg::metrics::Probability::removeAssignment could not find assignment and remove it.");
            }
            else
            {
                mAssignments.erase(it);
            }
        }

    ProbabilityDensityFunction::Ptr Probability::getProbabilityDensityFunction() const
        {
            return mModelProbabilityDistribution;
        }

}
}