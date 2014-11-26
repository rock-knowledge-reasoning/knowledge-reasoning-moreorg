#ifndef OWLAPI_MODEL_OWL_DISJOINT_CLASSES_AXIOM_HPP
#define OWLAPI_MODEL_OWL_DISJOINT_CLASSES_AXIOM_HPP

#include <owl_om/owlapi/model/OWLNaryClassAxiom.hpp>

namespace owlapi {
namespace model {

class OWLDisjointClassesAxiom : public OWLNaryClassAxiom
{
public:
    OWLDisjointClassesAxiom(OWLClassExpressionPtrList classExpressions, OWLAnnotationList annotations = OWLAnnotationList())
        : OWLNaryClassAxiom(classExpressions, DisjointClasses, annotations)
    {}

    /**
     * Accept a visitor to visit this object, i.e. 
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_DISJOINT_CLASSES_AXIOM_HPP
