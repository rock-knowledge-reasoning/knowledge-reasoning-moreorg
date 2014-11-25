#ifndef OWLAPI_MODEL_OWL_INDIVIDUAL_AXIOM_HPP
#define OWLAPI_MODEL_OWL_INDIVIDUAL_AXIOM_HPP

#include <owl_om/owlapi/model/OWLLogicalAxiom.hpp>

namespace owlapi {
namespace model {

class OWLIndividualAxiom : public OWLLogicalAxiom
{
public:
    OWLIndividualAxiom(AxiomType type, OWLAnnotationList annotations)
        : OWLLogicalAxiom(type, annotations)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_INDIVIDUAL_AXIOM_HPP
