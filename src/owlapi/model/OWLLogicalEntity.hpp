#ifndef OWLAPI_MODEL_OWL_LOGICAL_ENTITY_HPP
#define OWLAPI_MODEL_OWL_LOGICAL_ENTITY_HPP

#include <owl_om/owlapi/model/OWLEntity.hpp>

namespace owlapi {
namespace model {

class OWLLogicalEntity : public OWLEntity
{
public:
    OWLLogicalEntity(const IRI& iri)
        : OWLEntity(iri)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_LOGICAL_ENTITY_HPP
