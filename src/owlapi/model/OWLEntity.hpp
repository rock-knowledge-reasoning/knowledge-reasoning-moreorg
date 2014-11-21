#ifndef OWL_API_MODEL_OWL_ENTITY_HPP
#define OWL_API_MODEL_OWL_ENTITY_HPP

#include <owl_om/owlapi/model/OWLNamedObject.hpp>

namespace owlapi {
namespace model {

/**
 * \class OWLEntity
 * \details Entities are the fundamental building blocks of OWL 2 ontologies, and they define the vocabulary — the named terms — of an ontology. In logic, the set of entities is usually said to constitute the signature of an ontology. Apart from entities, OWL 2 ontologies typically also contain literals, such as strings or integers.
 * \see http://www.w3.org/TR/owl2-syntax/#Entities.2C_Literals.2C_and_Anonymous_Individuals
 */
class OWLEntity : public OWLNamedObject
{
public:
    OWLEntity( const IRI& iri = IRI())
        : OWLNamedObject(iri)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_ENTITY_HPP
