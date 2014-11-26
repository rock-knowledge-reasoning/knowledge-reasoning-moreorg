#ifndef OWLAPI_MODEL_OWL_DISJOINT_UNION_AXIOM_HPP
#define OWLAPI_MODEL_OWL_DISJOINT_UNION_AXIOM_HPP

#include <owl_om/owlapi/model/OWLClass.hpp>
#include <owl_om/owlapi/model/OWLClassAxiom.hpp>

namespace owlapi {
namespace model {

/**
 * Represents a dijoint union axiom 
 * \see http://www.w3.org/TR/2009/REC-owl2-syntax-20091027/#Disjoint_Union_of_Class_Expressions
 */
class OWLDisjointUnionAxiom : public OWLClassAxiom
{
public: 
    OWLDisjointUnionAxiom(OWLClass::Ptr owlClass, OWLClassExpressionPtrList classExpressions, OWLAnnotationList annotations = OWLAnnotationList())
        : OWLClassAxiom(DisjointUnion, annotations)
    {}

    /**
     * Accept a visitor to visit this object, i.e.
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_DISJOINT_UNION_AXIOM_HPP
