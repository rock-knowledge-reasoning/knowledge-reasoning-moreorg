#ifndef OWLAPI_MODEL_OWL_OBJECT_EXACT_CARDINALITY_HPP
#define OWLAPI_MODEL_OWL_OBJECT_EXACT_CARDINALITY_HPP

#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLObjectPropertyExpression.hpp>
#include <owl_om/owlapi/model/OWLExactCardinalityRestriction.hpp>
#include <owl_om/Vocabulary.hpp>

namespace owlapi {
namespace model {

class OWLObjectExactCardinality : public OWLExactCardinalityRestriction
{
public:
    OWLObjectExactCardinality(OWLObjectPropertyExpression::Ptr property, uint32_t cardinality, const OWLQualification& qualification = owl_om::vocabulary::OWL::Thing())
        : OWLExactCardinalityRestriction( boost::dynamic_pointer_cast<OWLPropertyExpression>(property), cardinality, qualification)
    {}

    ClassExpressionType getClassExpressionType() { return OBJECT_EXACT_CARDINALITY; }

    bool isAnonymous() { return true; }
    bool isClassExpressionLiteral() { return false; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_OBJECT_EXACT_CARDINALITY_HPP
