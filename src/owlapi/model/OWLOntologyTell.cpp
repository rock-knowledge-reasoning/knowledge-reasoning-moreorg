#include "OWLOntologyTell.hpp"
#include <owl_om/owlapi/KnowledgeBase.hpp>
#include <owl_om/owlapi/OWLApi.hpp>
#include <base/Logging.hpp>

namespace owlapi {
namespace model {

OWLOntologyTell::OWLOntologyTell(OWLOntology::Ptr ontology)
    : mpOntology(ontology)
{}

void OWLOntologyTell::initializeDefaultClasses()
{
    getOWLClass(vocabulary::OWL::Class());
    getOWLClass(vocabulary::OWL::Thing());

//    // http://www.w3.org/TR/2009/REC-owl2-syntax-20091027/#Entity_Declarations_and_Typing
//    // Declarations for the built-in entities of OWL 2, listed in Table 5, are implicitly present in every OWL 2 ontology.
//    OWLClass thing(vocabulary::OWL::Thing());
//    // vocabulary::OWL::Nothing()
//    // ObjectProperty
//    // vocabulary::OWL::topObjectProperty()
//    // vocabulary::OWL::bottomObjectProperty()
//    // DataProperty
//    // vocubulary::OWL::topDataProperty()
//    // vocabulary::OWL::bottomDataProperty()
//    // Datatypes:
//    // vocabulary::RDFS::Literal()
//    // ... each datatype in OWL 2
//    // AnnotationProperty
//    // vocabulary::OWL::AnnotationProperty()
//
//    // Trigger instanciation of the following classes
//    getClassLazy(vocabulary::OWL::Class());

}

OWLClass::Ptr OWLOntologyTell::getOWLClass(const IRI& iri)
{
    LOG_WARN_S << "Retrieve class: " << iri;
    std::map<IRI, OWLClass::Ptr>::const_iterator it = mpOntology->mClasses.find(iri);
    if(it != mpOntology->mClasses.end())
    {
        return it->second;
    } else {
        OWLClass::Ptr klass(new OWLClass(iri));
        mpOntology->mClasses[iri] = klass;

        // Update kb
        mpOntology->kb()->getClassLazy(iri);

        return klass;
    }
}

OWLAnonymousIndividual::Ptr OWLOntologyTell::getOWLAnonymousIndividual(const IRI& iri)
{
    std::map<IRI, OWLAnonymousIndividual::Ptr>::const_iterator it = mpOntology->mAnonymousIndividuals.find(iri);
    if(it != mpOntology->mAnonymousIndividuals.end())
    {
        return it->second;
    } else {
        NodeID node(iri.toString(), true);
        OWLAnonymousIndividual::Ptr individual(new OWLAnonymousIndividual(node));
        mpOntology->mAnonymousIndividuals[iri] = individual;
        return individual;
    }
}

OWLNamedIndividual::Ptr OWLOntologyTell::getOWLNamedIndividual(const IRI& iri)
{
    std::map<IRI, OWLNamedIndividual::Ptr>::const_iterator it = mpOntology->mNamedIndividuals.find(iri);
    if(it != mpOntology->mNamedIndividuals.end())
    {
        return it->second;
    } else {
        OWLNamedIndividual::Ptr individual(new OWLNamedIndividual(iri));
        mpOntology->mNamedIndividuals[iri] = individual;

        //Update kb
        mpOntology->kb()->getInstanceLazy(iri);

        return individual;
    }
}

OWLObjectProperty::Ptr OWLOntologyTell::getOWLObjectProperty(const IRI& iri)
{
    std::map<IRI, OWLObjectProperty::Ptr>::const_iterator it = mpOntology->mObjectProperties.find(iri);
    if(it != mpOntology->mObjectProperties.end())
    {
        return it->second;
    } else {
        OWLObjectProperty::Ptr property(new OWLObjectProperty(iri));
        mpOntology->mObjectProperties[iri] = property;

        //Update kb
        mpOntology->kb()->getObjectPropertyLazy(iri);

        return property;
    }
}

OWLDataProperty::Ptr OWLOntologyTell::getOWLDataProperty(const IRI& iri)
{
    std::map<IRI, OWLDataProperty::Ptr>::const_iterator it = mpOntology->mDataProperties.find(iri);
    if(it != mpOntology->mDataProperties.end())
    {
        return it->second;
    } else {
        OWLDataProperty::Ptr property(new OWLDataProperty(iri));
        mpOntology->mDataProperties[iri] = property;

        //Update kb
        mpOntology->kb()->getDataPropertyLazy(iri);

        return property;
    }
}

OWLSubClassOfAxiom::Ptr OWLOntologyTell::subclassOf(const IRI& subclass, OWLClassExpression::Ptr superclass)
{
    OWLClass::Ptr e_subclass = getOWLClass(subclass);
    return subclassOf(subclass, superclass);
}

OWLSubClassOfAxiom::Ptr OWLOntologyTell::subclassOf(const IRI& subclass, const IRI& superclass)
{
    // All classes inherit from top concept, i.e. owl:Thing
    OWLClass::Ptr e_subclass = getOWLClass(subclass);
    OWLClass::Ptr e_superclass = getOWLClass(superclass);

    return subclassOf(e_subclass, e_superclass);
}

OWLSubClassOfAxiom::Ptr OWLOntologyTell::subclassOf(OWLClass::Ptr subclass, OWLClass::Ptr superclass)
{
    mpOntology->kb()->subclassOf(subclass->getIRI(), superclass->getIRI());
    return subclassOf(boost::dynamic_pointer_cast<OWLClassExpression>(subclass),boost::dynamic_pointer_cast<OWLClassExpression>(superclass));
}


OWLSubClassOfAxiom::Ptr OWLOntologyTell::subclassOf(OWLClassExpression::Ptr subclassExpression, OWLClassExpression::Ptr superclassExpression)
{
    OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom(subclassExpression, superclassExpression));
    mpOntology->mSubClassAxiomBySubPosition[subclassExpression].push_back(axiom);
    mpOntology->mSubClassAxiomBySuperPosition[superclassExpression].push_back(axiom);

    LOG_DEBUG_S << "Added SubClassOfAxiom:" << subclassExpression->toString() << " axiom: " << axiom->toString();
    return axiom;
}

void OWLOntologyTell::addAxiom(OWLAxiom::Ptr axiom)
{
    mpOntology->mAxiomsByType[axiom->getAxiomType()].push_back( axiom );
}

OWLClassAssertionAxiom::Ptr OWLOntologyTell::instanceOf(const IRI& instance, const IRI& classType)
{
    // Update reasoner kb
    mpOntology->kb()->instanceOf(instance, classType);

    // ClassAssertion
    OWLNamedIndividual::Ptr e_individual = getOWLNamedIndividual(instance);
    OWLClassExpression::Ptr e_class = getOWLClass(classType);
    OWLClassAssertionAxiom::Ptr axiom(new OWLClassAssertionAxiom(e_individual, e_class));

    // Register
    mpOntology->mClassAssertionAxiomsByClass[e_class].push_back(axiom);
    mpOntology->mClassAssertionAxiomsByIndividual[e_individual].push_back(axiom);
    mpOntology->mNamedIndividualAxioms[e_individual].push_back(axiom);

    LOG_DEBUG_S << "NamedIndividual '" << instance << "' of class: '" << classType << "'";
    return axiom;
}

void OWLOntologyTell::inverseFunctionalProperty(const IRI& property)
{
    mpOntology->kb()->inverseFunctionalProperty(property);
}

void OWLOntologyTell::transitiveProperty(const IRI& property)
{
    mpOntology->kb()->transitiveProperty(property);
}

void OWLOntologyTell::functionalObjectProperty(const IRI& property)
{
    mpOntology->kb()->functionalProperty(property, KnowledgeBase::OBJECT);
}

void OWLOntologyTell::functionalDataProperty(const IRI& property)
{
    mpOntology->kb()->functionalProperty(property, KnowledgeBase::DATA);
}

void OWLOntologyTell::relatedTo(const IRI& subject, const IRI& relation, const IRI& object)
{
    mpOntology->kb()->relatedTo(subject, relation, object);
}

void OWLOntologyTell::dataPropertyDomainOf(const IRI& relation, const IRI& classType)
{
    mpOntology->kb()->domainOf(relation, classType, KnowledgeBase::DATA);
}

void OWLOntologyTell::objectPropertyDomainOf(const IRI& relation, const IRI& classType)
{
    mpOntology->kb()->domainOf(relation, classType, KnowledgeBase::OBJECT);
}

void OWLOntologyTell::objectPropertyRangeOf(const IRI& relation, const IRI& classType)
{
    mpOntology->kb()->rangeOf(relation, classType, KnowledgeBase::OBJECT);
}

void OWLOntologyTell::inverseOf(const IRI& relation, const IRI& inverseType)
{
    mpOntology->kb()->inverseOf(relation, inverseType);
}

void OWLOntologyTell::valueOf(const IRI& instance, const IRI& dataProperty, OWLLiteral::Ptr literal)
{
    reasoner::factpp::DataValue dataValue = mpOntology->kb()->dataValue(literal->getValue(), literal->getType());
    mpOntology->kb()->valueOf(instance, dataProperty, dataValue);
}

} // end namespace model
} // end namespace owlapi
