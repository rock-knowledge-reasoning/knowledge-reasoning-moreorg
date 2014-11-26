#ifndef OWLAPI_MODEL_OBJECT_PROPERTY_AXIOM_HPP
#define OWLAPI_MODEL_OBJECT_PROPERTY_AXIOM_HPP

#include <owl_om/owlapi/model/OWLPropertyAxiom.hpp>

namespace owlapi {
namespace model {

class OWLObjectPropertyAxiom : public OWLPropertyAxiom
{
public:
    typedef boost::shared_ptr<OWLPropertyAxiom> Ptr;

    OWLObjectPropertyAxiom(AxiomType type, OWLAnnotationList annotations)
        : OWLPropertyAxiom(type, annotations)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OBJECT_PROPERTY_AXIOM_HPP
