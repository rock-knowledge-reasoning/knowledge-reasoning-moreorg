#ifndef OWLAPI_MODEL_OWL_OBJECT_MIN_CARDINALITY_HPP
#define OWLAPI_MODEL_OWL_OBJECT_MIN_CARDINALITY_HPP

#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLObjectPropertyExpression.hpp>
#include <owl_om/owlapi/model/OWLMinCardinalityRestriction.hpp>
#include <owl_om/Vocabulary.hpp>

namespace owlapi {
namespace model {

class OWLObjectMinCardinality : public OWLMinCardinalityRestriction
{
public:
    OWLObjectMinCardinality(OWLObjectPropertyExpression::Ptr property, uint32_t cardinality, const OWLQualification& qualification = owl_om::vocabulary::OWL::Thing())
        : OWLMinCardinalityRestriction( boost::dynamic_pointer_cast<OWLPropertyExpression>(property), cardinality, qualification)
    {}

    ClassExpressionType getClassExpressionType() const { return OBJECT_MIN_CARDINALITY; }
    bool isAnonymous() const { return true; }
    bool isClassExpressionLiteral() const { return false; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_OBJECT_MIN_CARDINALITY_HPP
