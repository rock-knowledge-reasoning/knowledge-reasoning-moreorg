#ifndef OWLAPI_MODEL_OWL_DECLARATION_AXIOM_HPP
#define OWLAPI_MODEL_OWL_DECLARATION_AXIOM_HPP

#include <owl_om/owlapi/model/OWLEntity.hpp>
#include <owl_om/owlapi/model/OWLAxiom.hpp>

namespace owlapi {
namespace model {

/**
 *  Represents a Declaration in the OWL 2 Specification.
 *
 *  A declaration axiom declares an entity in an ontology. It doesn't affect the
 *  logical meaning of the ontology.
 *  \see http://www.w3.org/TR/2009/REC-owl2-syntax-20091027/#Entity_Declarations_and_Typing
 */
class OWLDeclarationAxiom : public OWLAxiom
{
    OWLEntity::Ptr mEntity;

public:
    OWLDeclarationAxiom(OWLEntity::Ptr entity, OWLAnnotationList annotations = OWLAnnotationList())
        : OWLAxiom(Declaration, annotations)
        , mEntity(entity)
    {}

    OWLEntity::Ptr getEntity() const { return mEntity; }

    /**
     * Accept a visitor to visit this object, i.e. 
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_DECLARATION_AXIOM_HPP
