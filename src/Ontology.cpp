#include "Ontology.hpp"
#include "db/rdf/SopranoDB.hpp"
#include "db/rdf/Sparql.hpp"
#include <sstream>
#include <base/Logging.hpp>

#include <owl_om/OWLApi.hpp>
#include <owl_om/Vocabulary.hpp>

using namespace owlapi::model;

namespace owl_om {

Ontology::Ontology()
    : mSparqlInterface(0)
{}

Ontology::Ptr Ontology::fromFile(const std::string& filename)
{
    Ontology::Ptr ontology(new Ontology());
    ontology->mSparqlInterface = new db::SopranoDB(filename);
    ontology->reload();
    ontology->refresh();

    return ontology;
}

Ontology::~Ontology()
{
    delete mSparqlInterface;
}

db::query::Results Ontology::findAll(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object) const
{
    using namespace owlapi::model;
    db::query::Results results = mSparqlInterface->findAll(subject, predicate, object);
    LOG_DEBUG_S << "Results: " << results.toString();
    return results;
}

void Ontology::reload()
{
    using namespace owl_om::db::query;

    initializeDefaultClasses();

    {
        db::query::Results results = findAll(Subject(),vocabulary::RDF::type(),vocabulary::OWL::Class());
        ResultsIterator it(results);
        while(it.next())
        {
            owlapi::model::IRI subject = it[Subject()];
            declareSubClassOf(subject, vocabulary::OWL::Thing());
        }
    }

    // Identify restrictions -- will contain the id, should be anonymous
    // Since SPARQL cannot query directly for anonymous restrictions, we do
    // an incremental construction after querying all triple and filtering for
    // the one that are related to the restriction
    //
    // TODO: introduce rdf triple parser
    // http://www.w3.org/TR/owl-parsing/#subsec-streaming
    std::vector<owlapi::model::IRI> restrictions;
    {
        db::query::Results results = findAll(Subject(), Predicate(), Object());
        ResultsIterator it(results);
        while(it.next())
        {
            owlapi::model::IRI subject = it[Subject()];
            owlapi::model::IRI predicate = it[Predicate()];
            owlapi::model::IRI object = it[Object()];

            // Check for all type triples
            if(predicate == vocabulary::RDF::type())
            {
                // Creating classes
                if(object == vocabulary::OWL::Class())
                {
                    // already registered
                } else if( object == vocabulary::OWL::NamedIndividual())
                {
                    // search for class types, but exclude NamedIndividual 'class'
                    Results objects = findAll(subject, vocabulary::RDF::type(), Object());
                    ResultsIterator objectsIt(objects);
                    while(objectsIt.next())
                    {
                        owlapi::model::IRI classType = objectsIt[Object()];
                        if(classType != vocabulary::OWL::NamedIndividual())
                        {
                            LOG_DEBUG_S << "NamedIndividual '" << subject << "' of class: '" << classType << "'";
                            instanceOf(subject, classType);

                            // ClassAssertion
                            OWLNamedIndividual::Ptr e_individual = getOWLNamedIndividual(subject);
                            OWLClassExpression::Ptr e_class = getOWLClass(classType);
                            OWLClassAssertionAxiom::Ptr axiom(new OWLClassAssertionAxiom(e_individual, e_class));

                            // Register
                            mClassAssertionAxiomsByClass[e_class].push_back(axiom);
                            mClassAssertionAxiomsByIndividual[e_individual].push_back(axiom);
                            mNamedIndividualAxioms[e_individual].push_back(axiom);
                        } else {
                            LOG_DEBUG_S << "Skipping NamedIndividual " << classType;
                        }
                    }
                } else if(object == vocabulary::RDF::Property())
                {
                    throw std::runtime_error("Property '" + subject.toString() + "' neither object nor data property");
                } else if(object == vocabulary::OWL::DatatypeProperty())
                {
                    dataProperty(subject);
                    OWLProperty::Ptr property = getOWLDataProperty(subject);
                } else if (object == vocabulary::RDFS::Datatype())
                {
                    // introduces a new datatype
                } else if( object == vocabulary::OWL::ObjectProperty())
                {
                    objectProperty(subject);
                    OWLProperty::Ptr property = getOWLObjectProperty(subject);

                } else if( object == vocabulary::OWL::FunctionalProperty())
                {
                    // delayed handling
                } else if( object == vocabulary::OWL::InverseFunctionalProperty())
                {
                    inverseFunctionalProperty(subject);
                } else if(object == vocabulary::OWL::TransitiveProperty())
                {
                    transitiveProperty(subject);
                } else if( object == vocabulary::OWL::AnnotationProperty())
                {
                    LOG_DEBUG_S << "Annotation property '" << subject << "' ignored for reasoning";
                } else if( object == vocabulary::OWL::Restriction() )
                {
                    LOG_WARN_S << "Found restriction: " << subject;
                    restrictions.push_back(subject);
                }
            } else if(predicate == vocabulary::RDFS::subClassOf())
            {
                // add a new axiom SubClassOf(subject-translation
                // object-translation)
                // where subject-translation is the translation to a class description
                // and object-translation correspondingly
                // If subject is a named class, then use partial definition
                // Class(subject partial object-translation)
                // If this axiom already exists of the form Class(subject
                // partial e1 e2 ... en) -> add axiom so that Class(subject
                // partial e1 e2 ... en object-translation)
                subclassOf(subject, object);

                // All classes inherit from top concept, i.e. owl:Thing
                OWLClass::Ptr e_subject = getOWLClass(subject);
                OWLClass::Ptr e_object = getOWLClass(object);

                // Check if this is truely a class (or an AnonymousIndividual)
                if( mSubClassAxiomBySubPosition.end() != mSubClassAxiomBySubPosition.find(e_object))
                {
                    // This is a class
                    OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom( e_subject, e_object ));

                    mSubClassAxiomBySubPosition[e_subject].push_back(axiom);
                    mSubClassAxiomBySuperPosition[e_object].push_back(axiom);
                } else {
                    OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom( e_subject, e_object ));
                    // AnonymousIndividual
                    OWLAnonymousIndividual::Ptr e_object = getOWLAnonymousIndividual(object);

                    LOG_WARN_S << "Add anonymous " << object << " from s: " << subject << ", p: " << predicate << ", o: " << object;

                    mSubClassAxiomBySubPosition[e_subject].push_back(axiom);
                    mAnonymousIndividualAxioms[e_object].push_back(axiom);
                }
            } else if(predicate == vocabulary::RDFS::domain())
            {
                // domain of a property
                // add object to a class description and add this class to the
                // domains of the given property (if this property exists)
            } else if(predicate == vocabulary::RDFS::range())
            {
                // range of a property
                // add object to a class description if this is an object
                // property and convert to data range if this is a data property
                // and add this class the the
                // range of the given property (if
            } else if(predicate == vocabulary::RDFS::subPropertyOf())
            {
                // validate that subject and object have the same property type
                // add axiom to assert superproperty
            } else if(predicate == vocabulary::OWL::equivalentProperty())
            {
                // validate that subject and object have the same property type
                // add axiom to assert superproperty
            } else if(predicate == vocabulary::OWL::inverseOf())
            {
                // check that subject and object are object properties, if not
                // raise, else
            } else if(predicate == vocabulary::OWL::oneOf())
            {
                // object is a node representing a list of named individuals
                // if l is not a list (of named individuals) raise
            } else if(predicate == vocabulary::OWL::intersectionOf())
            {
                // add the axiom Class(x complete lt1 lt2 .. ltn)
                // where lt1 ... ltn ard the translated descriptions in the list
                // l
                // if l is not a list (of class descriptions) raise an error

            } else if(predicate == vocabulary::OWL::unionOf())
            {
                // add the axiom Class(x complete unionOf(lt1 lt2 ..ltn)
                // otherwise same as intersectionOf
            } else if(predicate == vocabulary::OWL::complementOf())
            {
                // add the axiom Class(x complete complementOf(nt))
                // where nt is the translation of object, if nt is not a class
                // description raise
            }
        }
    }

    // Properties
    // Delayed execution since we need to know whether we deal with an object or datatype property
    {
        Results results = findAll(Subject(), vocabulary::RDF::type(), vocabulary::OWL::FunctionalProperty());
        ResultsIterator it(results);
        while(it.next())
        {

            owlapi::model::IRI subject = it[Subject()];
            if( isSubclassOf(subject, vocabulary::OWL::ObjectProperty()))
            {
                functionalProperty(subject, OBJECT);
            } else if ( isSubclassOf(subject, vocabulary::OWL::DatatypeProperty()) )
            {
                functionalProperty(subject, DATA);
            } else {
                throw std::invalid_argument("Property '" + subject.toString() + "' is not a known object or data property");
            }
        }
    }

    // Restrictions
    // RDF/XML Syntax
    // <owl:Class rdf:about="Person">
    //   <rdfs:subClassOf>
    //     <owl:Restriction>
    //       <owl:onProperty rdf:resource="hasParent"/>
    //       <owl:qualifiedCardinality rdf:datatype="&xsd;nonNegativeInteger">
    //         1
    //       </owl:qualifiedCardinality>
    //       <owl:onClass rdf:resource="Male">
    //     </owl:Restriction>
    //   </rdfs:subClassOf>
    // </owl:Class>
    {
        using namespace owlapi::model;
        // Iterate through all restrictions
        // _n rdf:type Restriction <- exactly one
        // _n owl:onProperty p  <- exactly one for _n, otherwise throw
        // _n owl:minCardinality 1
        // _n owl:maxCardinality 2
        Results results = findAll(Subject(), Predicate(), Object());
        ResultsIterator it(results);

        std::map<owlapi::model::IRI, OWLCardinalityRestriction> cardinalityRestrictions;
        while(it.next())
        {
            owlapi::model::IRI restriction = it[Subject()];
            if( restrictions.end() == std::find(restrictions.begin(), restrictions.end(), restriction))
            {
                continue;
            }

            owlapi::model::IRI predicate = it[Predicate()];
            if(predicate == vocabulary::OWL::onProperty())
            {
                OWLObjectProperty::Ptr oProperty( new OWLObjectProperty( it[Object()] ));

                OWLCardinalityRestriction* cardinalityRestrictionPtr = &cardinalityRestrictions[restriction];
                if(cardinalityRestrictionPtr->getProperty())
                {
                    std::stringstream ss;
                    ss << "Restriction '" << restriction << "' applies to more than one property, but requires to be exactly one";
                    throw std::invalid_argument("owl_om::Ontology: " + ss.str() );
                }

                cardinalityRestrictions[restriction].setProperty(boost::dynamic_pointer_cast<OWLPropertyExpression>(oProperty));
                LOG_WARN_S << "Restriction '" << restriction << "' applies to: " << oProperty->toString();
                continue;
            }
            else if(predicate == vocabulary::OWL::minCardinality() || predicate == vocabulary::OWL::minQualifiedCardinality())
            {
                OWLCardinalityRestriction* cardinalityRestrictionPtr = &cardinalityRestrictions[restriction];
                assert(cardinalityRestrictionPtr);
                uint32_t cardinality = OWLLiteral::create( it[Object()].toString() )->getInteger();
                cardinalityRestrictionPtr->setCardinality(cardinality);
                cardinalityRestrictionPtr->setCardinalityRestrictionType(OWLCardinalityRestriction::MIN);
                continue;
            } else if(predicate == vocabulary::OWL::maxCardinality() || predicate == vocabulary::OWL::maxQualifiedCardinality())
            {
                OWLCardinalityRestriction* cardinalityRestrictionPtr = &cardinalityRestrictions[restriction];
                uint32_t cardinality = OWLLiteral::create( it[Object()].toString() )->getInteger();
                cardinalityRestrictionPtr->setCardinality(cardinality);
                cardinalityRestrictionPtr->setCardinalityRestrictionType(OWLCardinalityRestriction::MAX);
                continue;
            } else if(predicate == vocabulary::OWL::cardinality() || predicate == vocabulary::OWL::qualifiedCardinality())
            {
                OWLCardinalityRestriction* cardinalityRestrictionPtr = &cardinalityRestrictions[restriction];
                uint32_t cardinality = OWLLiteral::create( it[Object()].toString() )->getInteger();
                cardinalityRestrictionPtr->setCardinality(cardinality);
                cardinalityRestrictionPtr->setCardinalityRestrictionType(OWLCardinalityRestriction::EXACT);
                continue;
            } else if(predicate == vocabulary::OWL::someValuesFrom())
            {
            } else if(predicate == vocabulary::OWL::allValuesFrom())
            {
            } else if(predicate == vocabulary::OWL::hasValue())
            {
            } else if(predicate == vocabulary::OWL::onClass())
            {
                IRI qualification = it[Object()];
                cardinalityRestrictions[restriction].setQualification(qualification);
                continue;
            }
        }  // while(it.next())

        LOG_DEBUG_S << "Restrictions";
        std::map<owlapi::model::IRI, OWLCardinalityRestriction>::const_iterator cit = cardinalityRestrictions.begin();
        for(; cit != cardinalityRestrictions.end(); ++cit)
        {

            // Found cardinality restriction
            try {
                OWLCardinalityRestriction::Ptr cardinalityRestriction = cit->second.narrow();
                // Get anonymous node this restriction is responsible for
                OWLAnonymousIndividual::Ptr anonymousIndividual = getOWLAnonymousIndividual(cit->first);

                std::vector<OWLAxiom::Ptr>& axioms = mAnonymousIndividualAxioms[anonymousIndividual];
                // Iterate over related subclass axioms
                OWLSubClassOfAxiom::Ptr subclassAxiomPtr;
                while(true)
                {
                    std::vector<OWLAxiom::Ptr>::iterator it = axioms.begin();
                    bool subClassFound = false;
                    for(; it != axioms.end(); ++it)
                    {
                        if((*it)->getAxiomType() == OWLAxiom::SubClassOf)
                        {
                            subclassAxiomPtr = boost::dynamic_pointer_cast<OWLSubClassOfAxiom>(*it);
                            axioms.erase(it);;
                            subClassFound = true;
                            break;
                        }
                    }
                    if(!subClassFound)
                    {
                        break;
                    }

                    declareSubClassOf(subclassAxiomPtr->getSubClass(), cardinalityRestriction);
                } // end while
            } catch(const std::runtime_error& e)
            {
                LOG_ERROR_S << "Error handling restriction: '" << cit->first << "' -- " << e.what();
            }
        }

        // TODO: add ObjectCardinalityRestriction
        // allow to search for cardinality restrictions
        // per model
        // show change: add UnvailableResource
        // --> compute the missing fulfillment based on given cardinalities
    }

    loadObjectProperties();
}

std::vector<OWLCardinalityRestriction::Ptr> Ontology::getCardinalityRestrictions(owlapi::model::OWLClassExpression::Ptr ce)
{
    // In order to find a restriction for a given class
    //    1. check class assertions for individuals
    // -> 2. check subclass axioms for classes
    //      - find superclass definitions, collect all restrictions
    //        - (including the ones for the superclasses -- identify restrictions)
    std::vector<OWLSubClassOfAxiom::Ptr> subclassAxioms = mSubClassAxiomBySubPosition[ce];

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

std::vector<OWLCardinalityRestriction::Ptr> Ontology::getCardinalityRestrictions(const IRI& iri)
{
    // In order to find a restriction for a given class
    //    1. check class assertions for individuals
    // -> 2. check subclass axioms for classes
    //      - find superclass definitions, collect all restrictions
    //        - (including the ones for the superclasses -- identify restrictions)
    OWLClass::Ptr klass = getOWLClass(iri);
    return getCardinalityRestrictions(klass);
}

std::map<IRI, std::vector<OWLCardinalityRestriction::Ptr> > Ontology::getCardinalityRestrictions(const std::vector<IRI>& klasses)
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

OWLClass::Ptr Ontology::getOWLClass(const IRI& iri)
{
    std::map<IRI, OWLClass::Ptr>::const_iterator it = mClasses.find(iri);
    if(it != mClasses.end())
    {
        return it->second;
    } else {
        OWLClass::Ptr klass(new OWLClass(iri));
        mClasses[iri] = klass;
        return klass;
    }
}

OWLAnonymousIndividual::Ptr Ontology::getOWLAnonymousIndividual(const IRI& iri)
{
    std::map<IRI, OWLAnonymousIndividual::Ptr>::const_iterator it = mAnonymousIndividuals.find(iri);
    if(it != mAnonymousIndividuals.end())
    {
        return it->second;
    } else {
        NodeID node(iri.toString(), true);
        OWLAnonymousIndividual::Ptr individual(new OWLAnonymousIndividual(node));
        mAnonymousIndividuals[iri] = individual;
        return individual;
    }
}

OWLNamedIndividual::Ptr Ontology::getOWLNamedIndividual(const IRI& iri)
{
    std::map<IRI, OWLNamedIndividual::Ptr>::const_iterator it = mNamedIndividuals.find(iri);
    if(it != mNamedIndividuals.end())
    {
        return it->second;
    } else {
        OWLNamedIndividual::Ptr individual(new OWLNamedIndividual(iri));
        mNamedIndividuals[iri] = individual;
        return individual;
    }
}

OWLObjectProperty::Ptr Ontology::getOWLObjectProperty(const IRI& iri)
{
    std::map<IRI, OWLObjectProperty::Ptr>::const_iterator it = mObjectProperties.find(iri);
    if(it != mObjectProperties.end())
    {
        return it->second;
    } else {
        OWLObjectProperty::Ptr property(new OWLObjectProperty(iri));
        mObjectProperties[iri] = property;
        return property;
    }
}

OWLDataProperty::Ptr Ontology::getOWLDataProperty(const IRI& iri)
{
    std::map<IRI, OWLDataProperty::Ptr>::const_iterator it = mDataProperties.find(iri);
    if(it != mDataProperties.end())
    {
        return it->second;
    } else {
        OWLDataProperty::Ptr property(new OWLDataProperty(iri));
        mDataProperties[iri] = property;
        return property;
    }
}

void Ontology::initializeDefaultClasses()
{
    // http://www.w3.org/TR/2009/REC-owl2-syntax-20091027/#Entity_Declarations_and_Typing
    // Declarations for the built-in entities of OWL 2, listed in Table 5, are implicitly present in every OWL 2 ontology.
    OWLClass thing(vocabulary::OWL::Thing());
    // vocabulary::OWL::Nothing()
    // ObjectProperty
    // vocabulary::OWL::topObjectProperty()
    // vocabulary::OWL::bottomObjectProperty()
    // DataProperty
    // vocubulary::OWL::topDataProperty()
    // vocabulary::OWL::bottomDataProperty()
    // Datatypes:
    // vocabulary::RDFS::Literal()
    // ... each datatype in OWL 2
    // AnnotationProperty
    // vocabulary::OWL::AnnotationProperty()

    // Trigger instanciation of the following classes
    getClassLazy(vocabulary::OWL::Class());
}


void Ontology::loadObjectProperties()
{
    using namespace owl_om::db::query;

    IRIList objectProperties = allObjectProperties();
    IRIList::const_iterator cit = objectProperties.begin();
    for(; cit != objectProperties.end(); ++cit)
    {
        IRI relation = *cit;
        Results results = findAll(Subject(), relation, Object());
        ResultsIterator it(results);
        while(it.next())
        {
            owlapi::model::IRI subject = it[Subject()];
            owlapi::model::IRI object = it[Object()];

            LOG_DEBUG_S << subject << " " << relation << " " << object;
            relatedTo(subject, relation, object);
        }

        // Setting domain of property
        {
            Results domain = findAll(relation, vocabulary::RDFS::domain(), Object());
            if(!domain.empty())
            {
                ResultsIterator domainIt(domain);
                while(domainIt.next())
                {
                    owlapi::model::IRI classType = domainIt[Object()];
                    domainOf(relation, classType, OBJECT);
                }
            }
        }
        // Setting range of property
        {
            Results range = findAll(relation, vocabulary::RDFS::range(), Object());
            if(!range.empty())
            {
                ResultsIterator rangeIt(range);
                while(rangeIt.next())
                {
                    owlapi::model::IRI classType = rangeIt[Object()];
                    rangeOf(relation, classType, OBJECT);
                }
            }
        }
        // Setting inverse property
        {
            Results inverses = findAll(relation, vocabulary::OWL::inverseOf(), Object());
            if(!inverses.empty())
            {
                ResultsIterator inversesIt(inverses);
                while(inversesIt.next())
                {
                    owlapi::model::IRI inverseType = inversesIt[Object()];
                    inverseOf(relation, inverseType);

                }
            }
        }
    }
}

std::string Ontology::toString() const
{
    std::stringstream txt;
    txt << "Ontology:" << std::endl;
    txt << "Classes (TBox statements):" << std::endl;
    {
        IRIList classes = allClasses();
        IRIList::const_iterator cit = classes.begin();
        for(; cit != classes.end(); ++cit)
        {
            txt << *cit << std::endl;
        }
        txt << std::endl << std::endl;
    }
    txt << "- - - - - - - - - - - - - - - -" << std::endl;
    txt << "Instances (ABox statements):" << std::endl;
    {
        IRIList instances = allInstances();
        IRIList::const_iterator cit = instances.begin();
        for(; cit != instances.end(); ++cit)
        {
            txt << *cit << std::endl;
        }
    }
    txt << "- - - - - - - - - - - - - - - -" << std::endl;
    txt << "ObjectProperties:" << std::endl;
    {
        IRIList properties = allObjectProperties();
        IRIList::const_iterator cit = properties.begin();
        for(; cit != properties.end(); ++cit)
        {
            txt << *cit << std::endl;
        }
    }
    txt << "- - - - - - - - - - - - - - - -" << std::endl;
    txt << "DataProperties:" << std::endl;
    {
        IRIList properties = allDataProperties();
        IRIList::const_iterator cit = properties.begin();
        for(; cit != properties.end(); ++cit)
        {
            txt << *cit << std::endl;
        }
    }

    txt << "- - - - - - - - - - - - - - - -" << std::endl;
    txt << "LISP Based Representation:" << std::endl;
    txt << KnowledgeBase::toString();

    return txt.str();
}

void Ontology::addAxiom(OWLAxiom::Ptr axiom)
{
    mAxiomsByType[axiom->getAxiomType()].push_back( axiom );
}

void Ontology::declareSubClassOf(const IRI& subclass, const IRI& superclass)
{
    subclassOf(subject, vocabulary::OWL::Thing());

    // All classes inherit from top concept, i.e. owl:Thing
    OWLClass::Ptr e_subject = getOWLClass(subject);
    OWLClass::Ptr e_thing = getOWLClass(vocabulary::OWL::Thing());
    OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom( e_subject, e_thing ));

    LOG_DEBUG_S << "Declare class: " << e_subject->getIRI();

    mSubClassAxiomBySubPosition[e_subject].push_back(axiom);
    mSubClassAxiomBySuperPosition[e_thing].push_back(axiom);
    // mDeclarationsByEntity[e_subject] = OWLClassDeclaration
}

void Ontology::declareSubClassOf(OWLClassExpression::Ptr subclassExpression, OWLClassExpression::Ptr superclassExpression)
{
    OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom(subclassExpression, superclassExpression));
    mSubClassAxiomBySubPosition[subclassExpression].push_back(axiom);
    mSubClassAxiomBySuperPosition[superclassExpression].push_back(axiom);
    
    LOG_DEBUG_S << "Added SubClassOfAxiom:" << subclassExpression->toString() << " axiom: " << axiom->toString();
}


} // end namespace owl_om
