#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_GROUNDING_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_GROUNDING_HPP

#include <map>
#include <owlapi/OWLApi.hpp>

namespace organization_model {
namespace organization_model {

typedef std::map<owlapi::model::OWLRestriction::Ptr, std::vector<owlapi::model::IRI> > RequirementsGrounding;

/**
 * Grounding allows to map a requirement / restriction to a given set of
 * resources (via IRI)
 */
class Grounding
{
    /// Map the requirement to the associated resource(s)
    RequirementsGrounding mRequirementToResourceMap;

public:
    Grounding(const RequirementsGrounding& grounding);

    const RequirementsGrounding& getRequirementsGrounding() { return mRequirementToResourceMap; }

    /**
     * Check if grounding is complete
     */
    bool isComplete() const;

    /**
     * Check if IRIList contains ungrounded elements
     */
    bool isComplete(const std::vector<owlapi::model::IRI>& partialGrounding) const;

    /**
     * Provide the open ungrounded requirements
     * \return map of open groundings
     */
    RequirementsGrounding ungroundedRequirements() const;

    /**
     * Compute the IRI which represents an ungrounded resource
     * \return IRI to represent ungrounded resources
     */
    static owlapi::model::IRI ungrounded();

    /**
     * Compute string representation of the grounding
     * \return Stringified Grounding object
     */
    std::string toString() const;

};

} // end namespace organization_model
} // end namespace organization_model

#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_GROUNDING_HPP
