#ifndef OWLAPI_MODEL_OWL_ONTOLOGY_HPP
#define OWLAPI_MODEL_OWL_ONTOLOGY_HPP

#include <map>
#include <owl_om/owlapi/model/OWLClass.hpp>
#include <owl_om/owlapi/model/OWLNamedIndividual.hpp>
#include <owl_om/owlapi/model/OWLAnonymousIndividual.hpp>
#include <owl_om/owlapi/model/OWLObjectProperty.hpp>
#include <owl_om/owlapi/model/OWLDataProperty.hpp>
#include <owl_om/owlapi/model/OWLAxiom.hpp>
#include <owl_om/owlapi/model/OWLClassAssertionAxiom.hpp>
#include <owl_om/owlapi/model/OWLDeclarationAxiom.hpp>
#include <owl_om/owlapi/model/OWLSubClassOfAxiom.hpp>

namespace owlapi {

class KnowledgeBase;

namespace model {

class OWLOntologyTell;
class OWLOntologyAsk;

/**
 * Ontology
 */
class OWLOntology
{
    friend class OWLOntologyTell;
    friend class OWLOntologyAsk;

protected:
    // Mapping of iri to types
    std::map<IRI, OWLClass::Ptr> mClasses;
    std::map<IRI, OWLNamedIndividual::Ptr> mNamedIndividuals;
    std::map<IRI, OWLAnonymousIndividual::Ptr> mAnonymousIndividuals;
    std::map<IRI, OWLObjectProperty::Ptr> mObjectProperties;
    std::map<IRI, OWLDataProperty::Ptr> mDataProperties;

    /// General axiom map
    std::map<OWLAxiom::AxiomType, std::vector<OWLAxiom::Ptr> > mAxiomsByType;

    std::map<OWLClassExpression::Ptr, std::vector<OWLClassAssertionAxiom::Ptr> > mClassAssertionAxiomsByClass;

    std::map<OWLIndividual::Ptr, std::vector<OWLClassAssertionAxiom::Ptr> > mClassAssertionAxiomsByIndividual;

    std::map<OWLDataProperty::Ptr, std::vector<OWLAxiom::Ptr> > mDataPropertyAxioms;
    std::map<OWLObjectProperty::Ptr, std::vector<OWLAxiom::Ptr> > mObjectPropertyAxioms;
    std::map<OWLNamedIndividual::Ptr, std::vector<OWLAxiom::Ptr> > mNamedIndividualAxioms;
    /// Map of anonymous individual to all axioms the individual is involved into
    std::map<OWLAnonymousIndividual::Ptr, std::vector<OWLAxiom::Ptr> > mAnonymousIndividualAxioms;
    std::map<OWLEntity::Ptr, std::vector<OWLDeclarationAxiom::Ptr> > mDeclarationsByEntity;


    /// Map to access subclass axiom by a given subclass
    std::map<OWLClassExpression::Ptr, std::vector<OWLSubClassOfAxiom::Ptr> > mSubClassAxiomBySubPosition;
    /// Map to access subclass axiom by a given superclass
    std::map<OWLClassExpression::Ptr, std::vector<OWLSubClassOfAxiom::Ptr> > mSubClassAxiomBySuperPosition;

    KnowledgeBase* mpKnowledgeBase;

    KnowledgeBase* kb() { return mpKnowledgeBase; }

public:
    typedef boost::shared_ptr<OWLOntology> Ptr;

    void refresh();

    /**
     * Default constructor
     */
    OWLOntology();

    ~OWLOntology();
};

} // ane namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_ONTOLOGY_HPP
