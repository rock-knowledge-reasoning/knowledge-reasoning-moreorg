#ifndef OWLAPI_MODEL_OWL_CLASS_AXIOM_HPP
#define OWLAPI_MODEL_OWL_CLASS_AXIOM_HPP

#include <boost/shared_ptr.hpp>
#include <owl_om/owlapi/model/OWLLogicalAxiom.hpp>

namespace owlapi {
namespace model {

class OWLClassAxiom : public OWLLogicalAxiom
{
public:
    typedef boost::shared_ptr<OWLClassAxiom> Ptr;

    OWLClassAxiom(AxiomType type, OWLAnnotationList annotations)
        : OWLLogicalAxiom(type, annotations)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_CLASS_AXIOM_HPP
