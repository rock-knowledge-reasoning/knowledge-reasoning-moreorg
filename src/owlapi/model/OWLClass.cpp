#include "OWLClass.hpp"

namespace owlapi {
namespace model {

OWLClass::OWLClass(const IRI& iri)
    : OWLClassExpression()
    , OWLLogicalEntity(iri)
{
    isThing = (iri == owl_om::vocabulary::OWL::Thing());
    isNothing = (iri == owl_om::vocabulary::OWL::Nothing());
}

std::string OWLClass::toString() const 
{ 
    return OWLClassExpression::toString() + ", OWLClass: '" + getIRI().toString() + "'"; 
}

} // end namespace model
} // end namespace owlapi
