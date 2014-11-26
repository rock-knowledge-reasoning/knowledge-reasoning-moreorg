#ifndef OWLAPI_MODEL_DATA_PROPERTY_ASSERTION_AXIOM_HPP
#define OWLAPI_MODEL_DATA_PROPERTY_ASSERTION_AXIOM_HPP

#include <owl_om/owlapi/model/OWLPropertyAssertionAxiom.hpp>
#include <owl_om/owlapi/model/OWLLiteral.hpp>

namespace owlapi {
namespace model {

/**
 * Represents a triple to describe the associated property value for a given subject, e.g.
 * subject: Peter
 * property: hasLastName
 * object: "Pan"
 */
class OWLDataPropertyAssertionAxiom : public OWLPropertyAssertionAxiom
{
public:
    OWLDataPropertyAssertionAxiom(OWLIndividual::Ptr subject, OWLPropertyExpression::Ptr property, OWLLiteral::Ptr object, OWLAnnotationList annotations = OWLAnnotationList())
            :  OWLPropertyAssertionAxiom(subject, property, object, DataPropertyAssertion, annotations)
    {}

    /**
     * Accept a visitor to visit this object, i.e. 
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);

};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_DATA_PROPERTY_ASSERTION_AXIOM_HPP
