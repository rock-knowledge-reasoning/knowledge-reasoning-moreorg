#ifndef OWL_API_MODEL_OWL_OBJECT_EXACT_CARDINALITY_HPP
#define OWL_API_MODEL_OWL_OBJECT_EXACT_CARDINALITY_HPP

#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLCardinalityRestriction.hpp>

namespace owlapi {
namespace model {

class OWLObjectExactCardinality : public OWLCardinalityRestriction
{
public:
    OWLObjectExactCardinality(OWLObjectPropertyExpression::Ptr property, uint32_t cardinality, const OWLQualification& qualification)
        : OWLCardinalityRestriction( boost::dynamic_pointer_cast<OWLPropertyExpression>(property), cardinality, qualification, EXACT)
    {}

    ClassExpressionType getClassExpressionType() { return OBJECT_EXACT_CARDINALITY; }

    bool isAnonymous() { return true; }
    bool isClassExpressionLiteral() { return false; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_OBJECT_EXACT_CARDINALITY_HPP
