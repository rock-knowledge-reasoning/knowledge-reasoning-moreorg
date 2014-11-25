#ifndef OWLAPI_MODEL_OWL_NAMED_OBJECT_HPP
#define OWLAPI_MODEL_OWL_NAMED_OBJECT_HPP

#include <owl_om/owlapi/model/OWLObject.hpp>
#include <owl_om/owlapi/model/HasIRI.hpp>

namespace owlapi {
namespace model {

class OWLNamedObject : public OWLObject, HasIRI
{
    IRI mIRI;

public:
    OWLNamedObject( const IRI& iri = IRI())
        : mIRI(iri)
    {}
   
    IRI getIRI() const { return mIRI; }

    void setIRI(const IRI& iri) { mIRI = iri; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_NAMED_OBJECT_HPP
