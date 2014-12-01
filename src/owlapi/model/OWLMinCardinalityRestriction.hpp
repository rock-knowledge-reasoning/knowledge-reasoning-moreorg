#ifndef OWL_API_MODEL_MIN_CARDINALITY_RESTRICTION_HPP
#define OWL_API_MODEL_MIN_CARDINALITY_RESTRICTION_HPP

#include <owl_om/owlapi/model/OWLCardinalityRestriction.hpp>

namespace owlapi {
namespace model {

class OWLMinCardinalityRestriction : public OWLCardinalityRestriction
{
public:
    OWLMinCardinalityRestriction(OWLPropertyExpression::Ptr property, uint32_t cardinality, const OWLQualification& qualification)
        : OWLCardinalityRestriction(property, cardinality, qualification, OWLCardinalityRestriction::MIN)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_MIN_CARDINALITY_RESTRICTION_HPP
