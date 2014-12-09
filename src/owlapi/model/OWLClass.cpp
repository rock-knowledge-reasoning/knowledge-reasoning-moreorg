#include "OWLClass.hpp"

namespace owlapi {
namespace model {

OWLClass::OWLClass(const IRI& iri)
    : OWLClassExpression()
    , OWLLogicalEntity(iri)
{
    mIsThing = (iri == owl_om::vocabulary::OWL::Thing());
    mIsNothing = (iri == owl_om::vocabulary::OWL::Nothing());
}

std::string OWLClass::toString() const 
{ 
    return OWLClassExpression::toString() + ", OWLClass: '" + getIRI().toString() + "'"; 
}

} // end namespace model
} // end namespace owlapi
