#ifndef OWL_OM_DB_ONTOLOGY_HPP
#define OWL_OM_DB_ONTOLOGY_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <owl_om/KnowledgeBase.hpp>
#include <owl_om/db/rdf/SparqlInterface.hpp>
#include <owl_om/OWLApi.hpp>

//! Main enclosing namespace for OWL Organization Modelling
namespace owl_om {

class OrganizationModel;

/**
 * @class KnowledgeBase
 * @brief KnowledgeBase serves as main layer of abstraction for the underlying database
 *
 * @see http://www.w3.org/TR/owl-parsing/#sec-implementation
 */
class Ontology : public owl_om::KnowledgeBase
{
    friend class OrganizationModel;

    // Mapping of iri to type
    std::map<owlapi::model::IRI, owlapi::model::OWLClass::Ptr> mClasses;
    std::map<owlapi::model::IRI, owlapi::model::OWLNamedIndividual::Ptr> mNamedIndividuals;
    std::map<owlapi::model::IRI, owlapi::model::OWLAnonymousIndividual::Ptr> mAnonymousIndividuals;
    std::map<owlapi::model::IRI, owlapi::model::OWLObjectProperty::Ptr> mObjectProperties;
    std::map<owlapi::model::IRI, owlapi::model::OWLDataProperty::Ptr> mDataProperties;

    std::map<owlapi::model::OWLAxiom::AxiomType, std::vector<owlapi::model::OWLAxiom::Ptr> > mAxiomsByType;

    std::map<owlapi::model::OWLClassExpression::Ptr, std::vector<owlapi::model::OWLClassAssertionAxiom::Ptr> > mClassAssertionAxiomsByClass;

    std::map<owlapi::model::OWLIndividual::Ptr, std::vector<owlapi::model::OWLClassAssertionAxiom::Ptr> > mClassAssertionAxiomsByIndividual;

    std::map<owlapi::model::OWLDataProperty::Ptr, std::vector<owlapi::model::OWLAxiom::Ptr> > mDataPropertyAxioms;
    std::map<owlapi::model::OWLObjectProperty::Ptr, std::vector<owlapi::model::OWLAxiom::Ptr> > mObjectPropertyAxioms;
    std::map<owlapi::model::OWLNamedIndividual::Ptr, std::vector<owlapi::model::OWLAxiom::Ptr> > mNamedIndividualAxioms;
    /// Map of anonymous individual to all axiom the individual is involved into
    std::map<owlapi::model::OWLAnonymousIndividual::Ptr, std::vector<owlapi::model::OWLAxiom::Ptr> > mAnonymousIndividualAxioms;
    std::map<owlapi::model::OWLEntity::Ptr, std::vector<owlapi::model::OWLDeclarationAxiom::Ptr> > mDeclarationsByEntity;


    /// Map to access subclass axiom by a given subclass
    std::map<owlapi::model::OWLClassExpression::Ptr, std::vector<owlapi::model::OWLSubClassOfAxiom::Ptr> > mSubClassAxiomBySubPosition;
    /// Map to access subclass axiom by a given superclass
    std::map<owlapi::model::OWLClassExpression::Ptr, std::vector<owlapi::model::OWLSubClassOfAxiom::Ptr> > mSubClassAxiomBySuperPosition;

public:
    /**
     * Contructs an empty ontology
     * Prefer using Ontology::fromFile
     */
    Ontology();

    /**
     * Deconstructor implementation
     */
    ~Ontology();

    typedef boost::shared_ptr<Ontology> Ptr;

    /**
     * Create knowledge base from file
     */
    static Ontology::Ptr fromFile(const std::string& filename);

    /**
     * Update the ontology
     */
    void reload();

    /**
     * Find all items that match the query
     */
    db::query::Results findAll(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object) const;

    /**
     * Convert ontology to string representation
     */
    std::string toString() const;

    /**
     * Compute restrictions on classes
     */
    std::map<owlapi::model::IRI, std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> > getCardinalityRestrictions(const std::vector<owlapi::model::IRI>& klasses);

    owlapi::model::OWLClass::Ptr getOWLClass(const IRI& iri);
    owlapi::model::OWLAnonymousIndividual::Ptr getOWLAnonymousIndividual(const IRI& iri);
    owlapi::model::OWLNamedIndividual::Ptr getOWLNamedIndividual(const IRI& iri);
    owlapi::model::OWLObjectProperty::Ptr getOWLObjectProperty(const IRI& iri);
    owlapi::model::OWLDataProperty::Ptr getOWLDataProperty(const IRI& iri);
private:

    /**
     * Initialize all default classes used in the ontology
     */
    void initializeDefaultClasses();

    /**
     * Load all object properties
     */
    void loadObjectProperties();

    /**
     * Add an axiom
     * \param axiom
     */
    void addAxiom(owlapi::model::OWLAxiom::Ptr axiom);

    db::query::SparqlInterface* mSparqlInterface;
};

} // end namespace owl_om

#endif // OWL_OM_DB_ONTOLOGY_HPP
