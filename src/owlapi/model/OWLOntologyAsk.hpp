#ifndef OWLAPI_MODEL_OWL_ONTOLOGY_ASK_HPP
#define OWLAPI_MODEL_OWL_ONTOLOGY_ASK_HPP

#include <owl_om/owlapi/model/OWLOntology.hpp>
#include <owl_om/owlapi/model/OWLCardinalityRestriction.hpp>
#include <owl_om/owlapi/model/OWLLiteral.hpp>
#include <owl_om/owlapi/model/OWLDataProperty.hpp>

namespace owlapi {
namespace model {

class OWLOntologyAsk
{
    OWLOntology::Ptr mpOntology;

public:
    typedef boost::shared_ptr<OWLOntologyAsk> Ptr;

    OWLOntologyAsk(OWLOntology::Ptr ontology);

    /**
     * Get or create the OWLClass instance by IRI
     * \return OWLClass::Ptr
     * \throw std::runtime_error if object cannot be found in ontology
     */
    OWLClass::Ptr getOWLClass(const IRI& iri) const;

    /**
     * Get or create the OWLAnonymousIndividual by IRI
     * \return OWLAnonymousIndividual::Ptr
     * \throw std::runtime_error if object cannot be found in ontology
     */
    OWLAnonymousIndividual::Ptr getOWLAnonymousIndividual(const IRI& iri) const;

    /**
     * Get or create the OWLNamedIndividual by IRI
     * \return OWLNamedIndividual::Ptr
     * \throw std::runtime_error if object cannot be found in ontology
     */
    OWLNamedIndividual::Ptr getOWLNamedIndividual(const IRI& iri) const;

    /**
     * Get or create the OWLObjectProperty by IRI
     * \return OWLObjectProperty::Ptr
     * \throw std::runtime_error if object cannot be found in ontology
     */
    OWLObjectProperty::Ptr getOWLObjectProperty(const IRI& iri) const;

    /**
     * Get or create the OWLDataProperty by IRI
     * \return OWLDataProperty::Ptr
     * \throw std::runtime_error if object cannot be found in ontology
     */
    OWLDataProperty::Ptr getOWLDataProperty(const IRI& iri) const;


    std::vector<OWLCardinalityRestriction::Ptr> getCardinalityRestrictions(owlapi::model::OWLClassExpression::Ptr ce) const;
    std::vector<OWLCardinalityRestriction::Ptr> getCardinalityRestrictions(const IRI& iri) const;
    std::map<IRI, std::vector<OWLCardinalityRestriction::Ptr> > getCardinalityRestrictions(const std::vector<IRI>& klasses) const;

    /**
     * Check if object identified by iri is a subclass of the given superclass
     * \param iri
     * \param superclass
     */
    bool isSubclassOf(const IRI& iri, const IRI& superclass) const;

    /**
     * Check if iri represents a known class
     */
    bool isOWLClass(const IRI& iri) const;

    /**
     * Retrieve all known classes
     * \return list of all classes
     */
    IRIList allClasses(bool excludeBottomClass = true) const;

    /**
     * Retrieve all subclasses of a given klass
     * \return list of all subclasses
     */
    IRIList allSubclassesOf(const IRI& klass, bool direct = false);

    /**
     * Retrieve all known instance of the given class type
     */
    IRIList allInstancesOf(const IRI& classType, bool direct = false);

    /**
     * Retrieve all known object properties
     */
    IRIList allObjectProperties() const;

    /**
     * Test if instance is type of a class
     * \param instance Instance identifier
     * \param klass Class identifier
     * \return true if instance is of given klass type, false otherwise
     */
    bool isInstanceOf(const IRI& instance, const IRI& klass);

    /**
     * Test if instances are related via given a given property
     * \param instance Instance identifier
     * \param relationProperty relation identifier:
     */
    bool isRelatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance) const;

    IRI typeOf(const IRI& instance) const;

    /**
     * Retrieve all related instances regarding a single instance and a selected property
     * which have a certain klass type
     * \param instance Instance name
     * \param relationProperty the relation property
     * \param klass Klass type the relation should map to
     * \return list of instances that are related to instance via the relationProperty
     */
    IRIList allRelatedInstances(const IRI& instance, const IRI& relationProperty, const IRI& klass = IRI());

    /**
     * Retrieve all instances regarding a single instance and a selected property
     * \param instance Instance where this relation is bound to
     * \param relationProperty Property for this relation (which will be inverted)
     * \param klass Optional filter klass for the result list, check if result is instance of given klass
     * \return list of instance that are related to instance via the inverse of the given 
     * relationProperty
     */
    IRIList allInverseRelatedInstances(const IRI& instance, const IRI& relationProperty, const IRI& klass = IRI());

    /**
     * Retrieve data value associated with instance
     * \return data value
     */
    OWLLiteral::Ptr getDataValue(const IRI& instance, const IRI& dataProperty);

    /**
     * Retrieve the list of ancestors
     * \return list of ancestors
     */
    IRIList ancestors(const IRI& instance) const;
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_ONTOLOGY_ASK_HPP
