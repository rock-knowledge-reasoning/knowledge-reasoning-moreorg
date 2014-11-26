#ifndef OWLAPI_MODEL_OWL_SUBCLASS_OF_AXIOM_HPP
#define OWLAPI_MODEL_OWL_SUBCLASS_OF_AXIOM_HPP

#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLClassAxiom.hpp>

namespace owlapi {
namespace model {

class OWLSubClassOfAxiom : public OWLClassAxiom
{
    OWLClassExpression::Ptr mSubClass;
    OWLClassExpression::Ptr mSuperClass;
public:
    typedef boost::shared_ptr<OWLSubClassOfAxiom> Ptr;

    OWLSubClassOfAxiom(OWLClassExpression::Ptr subClass, OWLClassExpression::Ptr superClass, OWLAnnotationList annotations = OWLAnnotationList())
        : OWLClassAxiom(OWLAxiom::SubClassOf, annotations)
        , mSubClass(subClass)
        , mSuperClass(superClass)
    {}

    /**
     * Accept a visitor to visit this object, i.e. 
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);

    OWLClassExpression::Ptr getSubClass() const { return mSubClass; }
    OWLClassExpression::Ptr getSuperClass() const { return mSuperClass; }

    /**
     * Determines if this subclass axiom has a subclass that is anonymous.
     */
    bool isGCI() const { throw std::runtime_error("OWLSubClassOfAxiom::isGCI: not implemented"); }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_SUBCLASS_OF_AXIOM_HPP
