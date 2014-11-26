#ifndef OWL_API_MODEL_OWL_CLASS_HPP
#define OWL_API_MODEL_OWL_CLASS_HPP

#include <owl_om/Vocabulary.hpp>
#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLLogicalEntity.hpp>

namespace owlapi {
namespace model {

/**
 * \brief OWLClass
 * \see http://www.w3.org/TR/owl2-syntax/#Class_Expression_Axioms
 */
class OWLClass : public OWLClassExpression, public OWLLogicalEntity
{
    bool isThing;
    bool isNothing;

public:
    typedef boost::shared_ptr<OWLClass> Ptr;

    OWLClass(const IRI& iri)
        : OWLClassExpression()
        , OWLLogicalEntity(iri)
    {
        isThing = (iri == owl_om::vocabulary::OWL::Thing());
        isNothing = (iri == owl_om::vocabulary::OWL::Nothing());
    }

    bool isTopEntity() const { return isThing; }
    bool isBottomEntity() const { return isNothing; }

    OWLClassExpression::ClassExpressionType getClassExpressionType() { return OWLClassExpression::OWL_CLASS; }

    bool isAnonymous() { return false; }

    bool isClassExpressionLiteral() { return true; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_CLASS_HPP
