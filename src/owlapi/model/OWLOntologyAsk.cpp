#include "OWLOntologyAsk.hpp"
#include <owl_om/owlapi/KnowledgeBase.hpp>

namespace owlapi {
namespace model {

OWLOntologyAsk::OWLOntologyAsk(OWLOntology::Ptr ontology)
    : mpOntology(ontology)
{}

OWLClass::Ptr OWLOntologyAsk::getOWLClass(const IRI& iri) const
{
    std::map<IRI, OWLClass::Ptr>::const_iterator it = mpOntology->mClasses.find(iri);
    if(it != mpOntology->mClasses.end())
    {
        return it->second;
    }

    throw std::runtime_error("OWLOntologyAsk::getOWLClass: '" + iri.toString() + "' is not a known OWLClass");
}

OWLAnonymousIndividual::Ptr OWLOntologyAsk::getOWLAnonymousIndividual(const IRI& iri) const
{
    std::map<IRI, OWLAnonymousIndividual::Ptr>::const_iterator it = mpOntology->mAnonymousIndividuals.find(iri);
    if(it != mpOntology->mAnonymousIndividuals.end())
    {
        return it->second;
    }
    throw std::runtime_error("OWLOntologyAsk::getAnonymousIndividual: '" + iri.toString() + "' is not a known OWLAnonymousIndividual");
}

OWLNamedIndividual::Ptr OWLOntologyAsk::getOWLNamedIndividual(const IRI& iri) const
{
    std::map<IRI, OWLNamedIndividual::Ptr>::const_iterator it = mpOntology->mNamedIndividuals.find(iri);
    if(it != mpOntology->mNamedIndividuals.end())
    {
        return it->second;
    } 

    throw std::runtime_error("OWLOntologyAsk::getNamedIndividual: '" + iri.toString() + "' is not a known OWLNamedIndividual");
}

OWLObjectProperty::Ptr OWLOntologyAsk::getOWLObjectProperty(const IRI& iri) const
{
    std::map<IRI, OWLObjectProperty::Ptr>::const_iterator it = mpOntology->mObjectProperties.find(iri);
    if(it != mpOntology->mObjectProperties.end())
    {
        return it->second;
    }
    throw std::runtime_error("OWLOntologyAsk::getOWLObjectProperty: '" + iri.toString() + "' is not a known OWLObjectProperty");
}

OWLDataProperty::Ptr OWLOntologyAsk::getOWLDataProperty(const IRI& iri) const
{
    std::map<IRI, OWLDataProperty::Ptr>::const_iterator it = mpOntology->mDataProperties.find(iri);
    if(it != mpOntology->mDataProperties.end())
    {
        return it->second;
    }

    throw std::runtime_error("OWLOntologyAsk::getOWLDataProperty: '" + iri.toString() + "' is not a known OWLDataProperty");
}

std::vector<OWLCardinalityRestriction::Ptr> OWLOntologyAsk::getCardinalityRestrictions(owlapi::model::OWLClassExpression::Ptr ce) const
{
    // In order to find a restriction for a given class
    //    1. check class assertions for individuals
    // -> 2. check subclass axioms for classes
    //      - find superclass definitions, collect all restrictions
    //        - (including the ones for the superclasses -- identify restrictions)
    std::vector<OWLSubClassOfAxiom::Ptr> subclassAxioms = mpOntology->mSubClassAxiomBySubPosition[ce];

    std::vector<OWLCardinalityRestriction::Ptr> restrictions;
    std::vector<OWLSubClassOfAxiom::Ptr>::const_iterator sit = subclassAxioms.begin();
    for(; sit != subclassAxioms.end(); ++sit)
    {
        OWLSubClassOfAxiom::Ptr subclassAxiomPtr = *sit;
        OWLClassExpression::Ptr superClass = subclassAxiomPtr->getSuperClass();
        OWLClassExpression::Ptr subClass = subclassAxiomPtr->getSubClass();

        switch(superClass->getClassExpressionType())
        {
            case OWLClassExpression::OBJECT_MIN_CARDINALITY:
            case OWLClassExpression::OBJECT_MAX_CARDINALITY:
            case OWLClassExpression::OBJECT_EXACT_CARDINALITY:
            case OWLClassExpression::DATA_EXACT_CARDINALITY:
            case OWLClassExpression::DATA_MIN_CARDINALITY:
            case OWLClassExpression::DATA_MAX_CARDINALITY:
                restrictions.push_back(boost::dynamic_pointer_cast<OWLCardinalityRestriction>(superClass));
                break;
            case OWLClassExpression::OWL_CLASS:
            {
                std::vector<OWLCardinalityRestriction::Ptr> inheritedRestrictions =    getCardinalityRestrictions(superClass);
                restrictions.insert(restrictions.end(), inheritedRestrictions.begin(), inheritedRestrictions.end());
            }
            default:
                break;
        }
    }
    return restrictions;
}

std::vector<OWLCardinalityRestriction::Ptr> OWLOntologyAsk::getCardinalityRestrictions(const IRI& iri) const
{
    // In order to find a restriction for a given class
    //    1. check class assertions for individuals
    // -> 2. check subclass axioms for classes
    //      - find superclass definitions, collect all restrictions
    //        - (including the ones for the superclasses -- identify restrictions)
    OWLClass::Ptr klass = getOWLClass(iri);
    return getCardinalityRestrictions(klass);
}

std::map<IRI, std::vector<OWLCardinalityRestriction::Ptr> > OWLOntologyAsk::getCardinalityRestrictions(const std::vector<IRI>& klasses) const
{
    std::vector<IRI>::const_iterator cit = klasses.begin();
    std::map<IRI, std::vector<OWLCardinalityRestriction::Ptr> > restrictionsMap;

    for(; cit != klasses.end(); ++cit)
    {
        IRI iri = *cit;
        restrictionsMap[iri] = getCardinalityRestrictions(iri);
    }
    return restrictionsMap;
}

bool OWLOntologyAsk::isSubclassOf(const IRI& iri, const IRI& superclass) const
{
    return mpOntology->kb()->isSubclassOf(iri, superclass);
}

bool OWLOntologyAsk::isOWLClass(const IRI& iri) const
{
    std::map<IRI, OWLClass::Ptr>::const_iterator it = mpOntology->mClasses.find(iri);
    return it != mpOntology->mClasses.end();
}

IRIList OWLOntologyAsk::allInstancesOf(const IRI& classType, bool direct)
{
    return mpOntology->kb()->allInstancesOf(classType, direct);
}

IRIList OWLOntologyAsk::allClasses(bool excludeBottomClass) const
{
    return mpOntology->kb()->allClasses(excludeBottomClass);
}

IRIList OWLOntologyAsk::allObjectProperties() const
{
    IRIList list;
    std::map<IRI, OWLObjectProperty::Ptr>::const_iterator cit = mpOntology->mObjectProperties.begin();
    for(; cit != mpOntology->mObjectProperties.end(); ++cit)
    {
        list.push_back(cit->first);
    }
    return list;
}

bool OWLOntologyAsk::isInstanceOf(const IRI& instance, const IRI& klass)
{
    return mpOntology->kb()->isInstanceOf(instance, klass);
}

bool OWLOntologyAsk::isRelatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance) const
{
    return mpOntology->kb()->isRelatedTo(instance, relationProperty, otherInstance);
}

IRI OWLOntologyAsk::typeOf(const IRI& instance) const
{
    return mpOntology->kb()->typeOf(instance);
}

IRIList OWLOntologyAsk::allRelatedInstances(const IRI& instance, const IRI& relationProperty, const IRI& klass)
{
    return mpOntology->kb()->allRelatedInstances(instance, relationProperty, klass);
}

IRIList OWLOntologyAsk::allInverseRelatedInstances(const IRI& instance, const IRI& relationProperty, const IRI& klass)
{
    return mpOntology->kb()->allInverseRelatedInstances(instance, relationProperty, klass);
}


OWLLiteral::Ptr OWLOntologyAsk::getDataValue(const IRI& instance, const IRI& dataProperty)
{
    reasoner::factpp::DataValue dataValue = mpOntology->kb()->getDataValue(instance, dataProperty);

    std::string valueRepresentation = dataValue.getValue();
    if(!dataValue.getType().empty())
    {
        valueRepresentation += "^^" + dataValue.getType();
    }

    return OWLLiteral::create(valueRepresentation);
}

} // end namespace model
} // end namespace owlapi
