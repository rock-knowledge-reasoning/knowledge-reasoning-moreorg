#include <owlapi/OWLApi.hpp>
#include <owlapi/model/IRI.hpp>
#include "../ResourceInstance.hpp"
#include <vector>
#include "../reasoning/ModelBound.hpp"

namespace moreorg
{
namespace metrics
{
    class Probability
    {
        typedef std::vector<Probability> List;
        protected:
            reasoning::ModelBound mRequirement;
            ResourceInstance::List mAssignments;
        public:
            Probability(const reasoning::ModelBound &requirement, const ResourceInstance::List &assignments);
            /**
            * Get the qualification, i.e. the model name
            * \return qualification
            */
            owlapi::model::IRI getQualification() const {return mRequirement.model;}
            void addAssignment(ResourceInstance &assignment) { mAssignments.push_back(assignment); }
            void removeAssignment(ResourceInstance &assignment);
            ResourceInstance::List getAssignments() const { return mAssignments; }

    };
}
}