#ifndef OWLAPI_MODEL_OWL_LOGICAL_AXIOM_HPP
#define OWLAPI_MODEL_OWL_LOGICAL_AXIOM_HPP

#include <owl_om/owlapi/model/OWLAxiom.hpp>

namespace owlapi {
namespace model {

/**
 * \brief A base interface of all axioms that affect the logical meaning of an ontology. This excludes declaration axioms (including imports declarations) and annotation axioms.
 */
class OWLLogicalAxiom : public OWLAxiom
{
public:
    OWLLogicalAxiom(AxiomType type, OWLAnnotationList annotations)
        : OWLAxiom(type, annotations)
    {}
    
    bool isLogicalAxiom() const { return true; } 
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_LOGICAL_AXIOM_HPP
