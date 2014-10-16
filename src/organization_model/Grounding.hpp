#ifndef OWL_OM_ORGANIZATION_MODEL_GROUNDING_HPP
#define OWL_OM_ORGANIZATION_MODEL_GROUNDING_HPP

#include <map>
#include <owl_om/KnowledgeBase.hpp>

namespace owl_om {
namespace organization_model {

typedef std::map<IRI,IRI> RequirementsGrounding;

class Grounding
{
    RequirementsGrounding mRequirementToResourceMap;
public:
    Grounding(const RequirementsGrounding& grounding);

    const RequirementsGrounding& getRequirementsGrounding() { return mRequirementToResourceMap; }

    bool isComplete() const;

    /**
     */
    IRIList ungroundedRequirements() const;

    static IRI ungrounded();

    std::string toString() const;
};

} // end namespace organization_model
} // end namespace owl_om
#endif // OWL_OM_ORGANIZATION_MODEL_GROUNDING_HPP
