#ifndef OWLAPI_MODEL_OBJECT_PROPERTY_ASSERTION_AXIOM_HPP
#define OWLAPI_MODEL_OBJECT_PROPERTY_ASSERTION_AXIOM_HPP

#include <owl_om/owlapi/model/OWLPropertyAssertionAxiom.hpp>

namespace owlapi {
namespace model {

class OWLObjectPropertyAssertionAxiom : public OWLPropertyAssertionAxiom
{
public:
    OWLObjectPropertyAssertionAxiom(OWLIndividual::Ptr subject, OWLPropertyExpression::Ptr property, OWLPropertyAssertionObject::Ptr object, OWLAnnotationList annotations = OWLAnnotationList())
            :  OWLPropertyAssertionAxiom(subject, property, object, ObjectPropertyAssertion, annotations)
    {}

    /**
     * Accept a visitor to visit this object, i.e. 
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OBJECT_PROPERTY_ASSERTION_AXIOM_HPP
