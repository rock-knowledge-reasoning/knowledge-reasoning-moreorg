#include "Ontology.hpp"
#include "db/rdf/SopranoDB.hpp"
#include "db/rdf/Sparql.hpp"
#include <sstream>
#include <base/Logging.hpp>

#include <owl_om/OWLApi.hpp>

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
            subclassOf(subject, vocabulary::OWL::Thing());

            // All classes inherit from top concept, i.e. owl:Thing
            OWLClass::Ptr e_subject(new OWLClass(subject));
            OWLClass::Ptr e_thing(new OWLClass(vocabulary::OWL::Thing()));
            OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom( e_subject, e_thing ));

            mSubClassAxiomBySubPosition[e_subject].push_back(axiom);
            mSubClassAxiomBySuperPosition[e_thing].push_back(axiom);

            // mDeclarationsByEntity[e_subject] = OWLClassDeclaration
        }
    }

    // Identify restrictions -- will contain the id, should be anonymous
    // Since SPARQL cannot query directly for anonymous restrictions, we do
    // an incremental construction after querying all triple and filtering for
    // the one that are related to the restriction
    std::vector<owlapi::model::IRI> restrictions;
    {
        db::query::Results results = findAll(Subject(), Predicate(), Object());
        ResultsIterator it(results);
        while(it.next())
        {
            owlapi::model::IRI subject = it[Subject()];
            owlapi::model::IRI predicate = it[Predicate()];
            owlapi::model::IRI object = it[Object()];

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
                            LOG_DEBUG_S << "Registering:" << classType;
                            instanceOf(subject, classType);

                            // ClassAssertion
                            OWLNamedIndividual::Ptr e_individual(new OWLNamedIndividual(subject));
                            OWLClassExpression::Ptr e_class(new OWLClass(classType));
                            OWLClassAssertionAxiom::Ptr axiom(new OWLClassAssertionAxiom(e_individual, e_class));

                            // Register
                            mClassAssertionAxiomsByClass[e_class].push_back(axiom);
                            mClassAssertionAxiomsByIndividual[e_individual].push_back(axiom);
                            mNamedIndividuals[e_individual].push_back(axiom);
                        } else {
                            LOG_DEBUG_S << "Skipping NamedIndividual " << classType;
                        }
                    }
                } else if (object == vocabulary::RDF::Property())
                {
                    throw std::runtime_error("Property '" + subject.toString() + "' neither object nor data property");
                } else if (object == vocabulary::OWL::DatatypeProperty())
                {
                    dataProperty(subject);
                    OWLProperty::Ptr property(new OWLDataProperty(subject));
                } else if ( object == vocabulary::OWL::ObjectProperty())
                {
                    objectProperty(subject);
                    OWLProperty::Ptr property(new OWLObjectProperty(subject));

                } else if ( object == vocabulary::OWL::FunctionalProperty())
                {
                    // delayed handling
                } else if ( object == vocabulary::OWL::InverseFunctionalProperty())
                {
                    inverseFunctionalProperty(subject);
                } else if (object == vocabulary::OWL::TransitiveProperty())
                {
                    transitiveProperty(subject);
                } else if ( object == vocabulary::OWL::AnnotationProperty())
                {
                    LOG_DEBUG_S << "Annotation property '" << subject << "' ignored for reasoning";
                } else if ( object == vocabulary::OWL::Restriction() )
                {
                    LOG_WARN_S << "Found restriction: " << subject;
                    restrictions.push_back(subject);
                }
            } else if(predicate == vocabulary::RDFS::subClassOf())
            {
                subclassOf(subject, object);

                // All classes inherit from top concept, i.e. owl:Thing
                OWLClass::Ptr e_subject(new OWLClass(subject));
                OWLClass::Ptr e_object(new OWLClass(object));

                // Check if this is truely a class (or an AnonymousIndividual)
                if( mSubClassAxiomBySubPosition.end() == mSubClassAxiomBySubPosition.find(e_object))
                {
                    OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom( e_subject, e_object ));

                    mSubClassAxiomBySubPosition[e_subject].push_back(axiom);
                    mSubClassAxiomBySuperPosition[e_object].push_back(axiom);
                } else {
                    OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom( e_subject, e_object ));

                    // AnonymousIndividual
                    NodeID node(object.toString(), true);
                    OWLAnonymousIndividual::Ptr e_object(new OWLAnonymousIndividual(node));

                    mSubClassAxiomBySubPosition[e_subject].push_back(axiom);
                    mAnonymousIndividuals[e_object].push_back(axiom);
                }
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
            } else if(predicate == vocabulary::OWL::maxCardinality() || predicate == vocabulary::OWL::qualifiedCardinality())
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

        LOG_INFO_S << "Restrictions";
        std::map<owlapi::model::IRI, OWLCardinalityRestriction>::const_iterator cit = cardinalityRestrictions.begin();
        for(; cit != cardinalityRestrictions.end(); ++cit)
        {
            // Found cardinality restriction
            OWLCardinalityRestriction::Ptr cardinalityRestriction = cit->second.narrow();

            // Get anonymous node this restriction is responsible for
            IRI anonymousNode = cit->first;
            NodeID node(anonymousNode.toString(), false);
            OWLAnonymousIndividual::Ptr anonymousIndividual(new OWLAnonymousIndividual(node));

            // Get related axioms
            std::vector<OWLAxiom::Ptr>& axioms = mAnonymousIndividuals[anonymousIndividual];
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
                OWLSubClassOfAxiom::Ptr axiom(new OWLSubClassOfAxiom(subclassAxiomPtr->getSubClass(), cardinalityRestriction));
                mSubClassAxiomBySubPosition[subclassAxiomPtr->getSubClass()].push_back(axiom);
                mSubClassAxiomBySuperPosition[cardinalityRestriction].push_back(axiom);
            } // end while
        }

        // In order to find a restriction for a given class
        // 1. check class assertions for individuals
        // 2. check subclass axioms for classes
        //      - find superclass definitions, collect all restrictions
        //        - (including the ones for the superclasses -- identify restrictions)



        // TODO: add ObjectCardinalityRestriction
        // allow to search for cardinality restrictions
        // per model
        // show change: add UnvailableResource
        // --> compute the missing fulfillment based on given cardinalities
    }

    loadObjectProperties();
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

} // end namespace owl_om
