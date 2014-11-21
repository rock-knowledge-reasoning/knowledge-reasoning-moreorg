#ifndef OWL_API_MODEL_OWL_OBJECT_PROPERTY_HPP
#define OWL_API_MODEL_OWL_OBJECT_PROPERTY_HPP

#include <owl_om/owlapi/model/OWLEntity.hpp>
#include <owl_om/owlapi/model/OWLObjectPropertyExpression.hpp>

namespace owlapi {
namespace model {

class OWLObjectProperty : public OWLObjectPropertyExpression, OWLEntity
{

public:
    OWLObjectProperty(const IRI& iri)
        : OWLEntity(iri)
    {}

};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_OBJECT_PROPERTY_HPP
