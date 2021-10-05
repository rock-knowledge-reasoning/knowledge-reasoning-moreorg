#include <owlapi/OWLApi.hpp>
#include <owlapi/model/IRI.hpp>
#include "../ResourceInstance.hpp"
#include <vector>
#include "../reasoning/ModelBound.hpp"
#include "DistributionFunctions.hpp"

namespace moreorg
{
    namespace metrics
    {
        class Probability
        {
        public:
            typedef std::vector<Probability> List;
            Probability(const reasoning::ModelBound &requirement, const ResourceInstance::List &assignments, const ProbabilityDensityFunction::Ptr &probabilityDensityFunction);
            Probability(const Probability &probability);
            /**
            * Get the qualification, i.e. the model name
            * \return qualification
            */

           ProbabilityDensityFunction::Ptr getProbabilityDensityFunction() const;

            
            owlapi::model::IRI getQualification() const { return mRequirement.model; }
            reasoning::ModelBound getRequirement() const { return mRequirement; }
            void addAssignment(ResourceInstance &assignment) { mAssignments.push_back(assignment); }
            void removeAssignment(ResourceInstance &assignment);
            ResourceInstance::List getAssignments() const { return mAssignments; }
        protected:
            reasoning::ModelBound mRequirement;
            ResourceInstance::List mAssignments;
            /// Probability Function to model underlying probability
            ProbabilityDensityFunction::Ptr mModelProbabilityDistribution;
        };
    }
}