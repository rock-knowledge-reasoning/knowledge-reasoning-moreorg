#ifndef OWLAPI_MODEL_OWL_CLASS_ASSERTION_AXIOM_HPP
#define OWLAPI_MODEL_OWL_CLASS_ASSERTION_AXIOM_HPP

#include <owl_om/owlapi/model/OWLIndividual.hpp>
#include <owl_om/owlapi/model/OWLIndividualAxiom.hpp>
#include <owl_om/owlapi/model/OWLClassExpression.hpp>

namespace owlapi {
namespace model {

class OWLClassAssertionAxiom : public OWLIndividualAxiom
{
    OWLIndividual::Ptr mpIndividual;
    OWLClassExpression::Ptr mpClassExpression;
public:
    OWLClassAssertionAxiom(OWLIndividual::Ptr individual, OWLClassExpression::Ptr classExpression, OWLAnnotationList annotations = OWLAnnotationList())
        : OWLIndividualAxiom(OWLAxiom::ClassAssertion, annotations)
        , mpIndividual(individual)
        , mpClassExpression(classExpression)
    {}

    OWLClassExpression::Ptr getClassExpression() const { return mpClassExpression; }
    OWLIndividual::Ptr getIndividual() const { return mpIndividual; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_CLASS_ASSERTION_AXIOM_HPP
