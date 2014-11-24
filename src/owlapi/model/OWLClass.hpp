#ifndef OWL_API_MODEL_OWL_CLASS_HPP
#define OWL_API_MODEL_OWL_CLASS_HPP

#include <owl_om/Vocabulary.hpp>
#include <owl_om/owlapi/model/OWLClassExpression.hpp>

namespace owlapi {
namespace model {

class OWLClass : public OWLClassExpression
{
    IRI mIRI;
    bool isThing;
    bool isNothing;

public:
    OWLClass(const IRI& iri)
        : OWLClassExpression()
        , mIRI(iri)
    {
        isThing = (iri == owl_om::vocabulary::OWL::Thing());
        isNothing = (iri == owl_om::vocabulary::OWL::Nothing());
    }

    bool isTopEntity() const { return isThing; }
    bool isBottomEntity() const { return isNothing; }

    OWLClassExpression::ClassExpressionType getClassExpressionType() { return OWLClassExpression::OWL_CLASS; }

    IRI getIRI() const { return mIRI; }

    bool isAnonymous() { return false; }

    bool isClassExpressionLiteral() { return true; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_CLASS_HPP
