#ifndef OWLAPI_MODEL_OWL_DATA_PROPERTY_AXIOM_HPP
#define OWLAPI_MODEL_OWL_DATA_PROPERTY_AXIOM_HPP

#include <owl_om/owlapi/model/OWLPropertyAxiom.hpp>

namespace owlapi {
namespace model {

/**
 *  A high level interface which marks data property axioms (e.g. Functional
 *  data property, data property domain etc.)
 */
class OWLDataPropertyAxiom : public OWLPropertyAxiom
{
public:
    OWLDataPropertyAxiom(AxiomType type, OWLAnnotationList annotations = OWLAnnotationList());
        : OWLPropertyAxiom(type, annotations)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_DATA_PROPERTY_AXIOM_HPP
