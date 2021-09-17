#include "Probability.hpp"


namespace moreorg {
namespace metrics {

    Probability::Probability(const reasoning::ModelBound &requirement, const ResourceInstance::List &assignments)
                            : mRequirement(requirement), mAssignments(assignments)
    {
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

}
}