#ifndef OWLAPI_MODEL_OWL_EQUIVALENT_CLASSES_AXIOM_HPP
#define OWLAPI_MODEL_OWL_EQUIVALENT_CLASSES_AXIOM_HPP

#include <stdexcept>
#include <owl_om/owlapi/model/OWLClassAxiom.hpp>
#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLClass.hpp>

namespace owlapi {
namespace model {

class OWLEquivalentClassesAxiom : public OWLClassAxiom
{
    OWLClassExpressionPtrList mClassExpressions;

public:
    OWLEquivalentClassesAxiom(OWLClassExpressionPtrList classExpressions, OWLAnnotationList annotations)
        : OWLClassAxiom(EquivalentClasses, annotations)
        , mClassExpressions(classExpressions)
    {}

    bool containsNamedEquivalentClass() const { throw std::runtime_error("OWLEquivalentClassesAxiom:containsOWLNothing: not implemented"); }

    bool containsOWLNothing() const { throw std::runtime_error("OWLEquivalentClassesAxiom:containsOWLNothing: not implemented"); }

    bool containsOWLThing() const { throw std::runtime_error("OWLEquivalentClassesAxiom:containsOWLThing: not implemented"); }

    std::vector<OWLClass> getNamedClasses() { throw std::runtime_error("OWLEquivalentClassesAxiom:getNamedClasses: not implemented"); }

    /**
     * Accept a visitor to visit this object, i.e.
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_EQUIVALENT_CLASSES_AXIOM_HPP
