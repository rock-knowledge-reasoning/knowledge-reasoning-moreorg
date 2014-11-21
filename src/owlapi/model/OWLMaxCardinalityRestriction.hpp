#ifndef OWL_API_MODEL_MAX_CARDINALITY_RESTRICTION_HPP
#define OWL_API_MODEL_MAX_CARDINALITY_RESTRICTION_HPP

#include <owl_om/owlapi/model/OWLCardinalityRestriction.hpp>

namespace owlapi {
namespace model {

class OWLMaxCardinalityRestriction : public OWLCardinalityRestriction
{
public:
    OWLMaxCardinalityRestriction(const OWLPropertyExpression& property, uint32_t cardinality, const OWLQualification& qualification)
        : OWLCardinalityRestriction(property, cardinality, qualification)
    {}

    CardinalityType getCardinalityType() const { return OWLCardinalityRestriction::MAX; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_MAX_CARDINALITY_RESTRICTION_HPP
