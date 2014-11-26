#ifndef OWLAPI_MODEL_PROPERTY_AXIOM_HPP
#define OWLAPI_MODEL_PROPERTY_AXIOM_HPP

#include <owl_om/owlapi/model/OWLLogicalAxiom.hpp>

namespace owlapi {
namespace model {

class OWLPropertyAxiom : public OWLLogicalAxiom
{
public:
    typedef boost::shared_ptr<OWLPropertyAxiom> Ptr;

    OWLPropertyAxiom(AxiomType type, OWLAnnotationList annotations)
        : OWLLogicalAxiom(type, annotations)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_PROPERTY_AXIOM_HPP
