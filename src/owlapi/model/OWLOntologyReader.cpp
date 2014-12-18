#include "OWLOntologyReader.hpp"
#include "OWLOntologyTell.hpp"

#include <owl_om/owlapi/db/rdf/SopranoDB.hpp>
#include <owl_om/owlapi/db/rdf/Sparql.hpp>
#include <owl_om/owlapi/Vocabulary.hpp>
#include <base/Logging.hpp>

namespace owlapi {
namespace model {

OWLOntologyReader::OWLOntologyReader()
    : mSparqlInterface(0)
    , mTell(0)
    , mAsk(0)
{}

OWLOntology::Ptr OWLOntologyReader::fromFile(const std::string& filename)
{
    OWLOntology::Ptr ontology(new OWLOntology());
    mSparqlInterface = new db::SopranoDB(filename);

    mTell = new OWLOntologyTell(ontology);
    mAsk = new OWLOntologyAsk(ontology);

    load();

    return ontology;
}

OWLOntologyReader::~OWLOntologyReader()
{
    delete mSparqlInterface;
    delete mTell;
}

db::query::Results OWLOntologyReader::findAll(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object) const
{
    db::query::Results results = mSparqlInterface->findAll(subject, predicate, object);
    LOG_DEBUG_S << "Results: " << results.toString();
    return results;
}

void OWLOntologyReader::load()
{
    using namespace owlapi::db::query;

    mTell->initializeDefaultClasses();

    {
        db::query::Results results = findAll(Subject(),vocabulary::RDF::type(),vocabulary::OWL::Class());
        ResultsIterator it(results);
        while(it.next())
        {
            IRI subject = it[Subject()];
            mTell->subclassOf(subject, vocabulary::OWL::Thing());
        }
    }

    // Identify restrictions -- will contain the id, should be anonymous
    // Since SPARQL cannot query directly for anonymous restrictions, we do
    // an incremental construction after querying all triple and filtering for
    // the one that are related to the restriction
    //
    // TODO: introduce rdf triple parser
    // http://www.w3.org/TR/owl-parsing/#subsec-streaming
    std::vector<IRI> restrictions;
    std::map<IRI, std::vector<OWLClass::Ptr> > anonymousRestrictions;
    {
        db::query::Results results = findAll(Subject(), Predicate(), Object());
        ResultsIterator it(results);
        while(it.next())
        {
            IRI subject = it[Subject()];
            IRI predicate = it[Predicate()];
            IRI object = it[Object()];

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
                        IRI classType = objectsIt[Object()];
                        if(classType != vocabulary::OWL::NamedIndividual())
                        {
                            mTell->instanceOf(subject, classType);

                        } else {
                            LOG_DEBUG_S << "Skipping NamedIndividual " << classType;
                        }
                    }
                } else if(object == vocabulary::RDF::Property())
                {
                    throw std::runtime_error("Property '" + subject.toString() + "' neither object nor data property");
                } else if(object == vocabulary::OWL::DatatypeProperty())
                {
                    mTell->getOWLDataProperty(subject);
                } else if (object == vocabulary::RDFS::Datatype())
                {
                    // introduces a new datatype
                } else if( object == vocabulary::OWL::ObjectProperty())
                {
                    mTell->getOWLObjectProperty(subject);

                } else if( object == vocabulary::OWL::FunctionalProperty())
                {
                    // delayed handling
                } else if( object == vocabulary::OWL::InverseFunctionalProperty())
                {
                    mTell->inverseFunctionalProperty(subject);
                } else if(object == vocabulary::OWL::TransitiveProperty())
                {
                    mTell->transitiveProperty(subject);
                } else if( object == vocabulary::OWL::AnnotationProperty())
                {
                    LOG_DEBUG_S << "Annotation property '" << subject << "' ignored for reasoning";
                } else if( object == vocabulary::OWL::Restriction() )
                {
                    LOG_DEBUG_S << "Found restriction: " << subject;
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
                // subclassOf(subject, object);

                // Check if this is truely a class (or an AnonymousIndividual)
                if( mAsk->isOWLClass(object) )
                {
                    // This is a class
                    mTell->subclassOf(subject, object);
                } else {
                    // We have to delay the mapping until the anonymous node has 
                    // been fully resolved to a restriction or similar
                    OWLClass::Ptr e_subject = mTell->getOWLClass(subject);
                    // cache the restrictions
                    anonymousRestrictions[object].push_back(e_subject);

                    LOG_DEBUG_S << "Add anonymous " << object << " from s: " << subject << ", p: " << predicate << ", o: " << object;
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
    // TODO Property check
    {
        Results results = findAll(Subject(), vocabulary::RDF::type(), vocabulary::OWL::FunctionalProperty());
        ResultsIterator it(results);
        while(it.next())
        {

            IRI subject = it[Subject()];
            if( mAsk->isSubclassOf(subject, vocabulary::OWL::ObjectProperty()))
            {
                mTell->functionalObjectProperty(subject);
            } else if ( mAsk->isSubclassOf(subject, vocabulary::OWL::DatatypeProperty()) )
            {
                mTell->functionalDataProperty(subject);
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
        // Iterate through all restrictions
        // _n rdf:type Restriction <- exactly one
        // _n owl:onProperty p  <- exactly one for _n, otherwise throw
        // _n owl:minCardinality 1
        // _n owl:maxCardinality 2
        Results results = findAll(Subject(), Predicate(), Object());
        ResultsIterator it(results);

        std::map<IRI, OWLCardinalityRestriction> cardinalityRestrictions;
        while(it.next())
        {
            IRI restriction = it[Subject()];
            if( restrictions.end() == std::find(restrictions.begin(), restrictions.end(), restriction))
            {
                continue;
            }

            IRI predicate = it[Predicate()];
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
        std::map<IRI, OWLCardinalityRestriction>::const_iterator cit = cardinalityRestrictions.begin();
        for(; cit != cardinalityRestrictions.end(); ++cit)
        {

            // Found cardinality restriction
            try {
                OWLCardinalityRestriction::Ptr cardinalityRestriction = cit->second.narrow();

                // Get anonymous node this restriction is responsible for
                std::vector<OWLClass::Ptr> subclasses = anonymousRestrictions[cit->first];
                std::vector<OWLClass::Ptr>::const_iterator sit = subclasses.begin();
                for(; sit != subclasses.end(); ++sit)
                {
                    mTell->subclassOf(*sit, cardinalityRestriction);
                }
            } catch(const std::runtime_error& e)
            {
                LOG_ERROR_S << "Error handling restriction: '" << cit->first << "' -- " << e.what();
            }
        }
    }

    loadObjectProperties();
}

void OWLOntologyReader::loadObjectProperties()
{
    using namespace db::query;

    IRIList objectProperties = mAsk->allObjectProperties();
    IRIList::const_iterator cit = objectProperties.begin();
    for(; cit != objectProperties.end(); ++cit)
    {
        IRI relation = *cit;
        Results results = findAll(Subject(), relation, Object());
        ResultsIterator it(results);
        while(it.next())
        {
            IRI subject = it[Subject()];
            IRI object = it[Object()];

            LOG_DEBUG_S << subject << " " << relation << " " << object;
            mTell->relatedTo(subject, relation, object);
        }

        // Setting domain of property
        {
            Results domain = findAll(relation, vocabulary::RDFS::domain(), Object());
            if(!domain.empty())
            {
                ResultsIterator domainIt(domain);
                while(domainIt.next())
                {
                    IRI classType = domainIt[Object()];
                    mTell->objectPropertyDomainOf(relation, classType);
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
                    IRI classType = rangeIt[Object()];
                    mTell->objectPropertyRangeOf(relation, classType);
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
                    IRI inverseType = inversesIt[Object()];
                    mTell->inverseOf(relation, inverseType);

                }
            }
        }
    }
}

//std::string Ontology::toString() const
//{
//    std::stringstream txt;
//    txt << "Ontology:" << std::endl;
//    txt << "Classes (TBox statements):" << std::endl;
//    {
//        IRIList classes = allClasses();
//        IRIList::const_iterator cit = classes.begin();
//        for(; cit != classes.end(); ++cit)
//        {
//            txt << *cit << std::endl;
//        }
//        txt << std::endl << std::endl;
//    }
//    txt << "- - - - - - - - - - - - - - - -" << std::endl;
//    txt << "Instances (ABox statements):" << std::endl;
//    {
//        IRIList instances = allInstances();
//        IRIList::const_iterator cit = instances.begin();
//        for(; cit != instances.end(); ++cit)
//        {
//            txt << *cit << std::endl;
//        }
//    }
//    txt << "- - - - - - - - - - - - - - - -" << std::endl;
//    txt << "ObjectProperties:" << std::endl;
//    {
//        IRIList properties = allObjectProperties();
//        IRIList::const_iterator cit = properties.begin();
//        for(; cit != properties.end(); ++cit)
//        {
//            txt << *cit << std::endl;
//        }
//    }
//    txt << "- - - - - - - - - - - - - - - -" << std::endl;
//    txt << "DataProperties:" << std::endl;
//    {
//        IRIList properties = allDataProperties();
//        IRIList::const_iterator cit = properties.begin();
//        for(; cit != properties.end(); ++cit)
//        {
//            txt << *cit << std::endl;
//        }
//    }
//
//    txt << "- - - - - - - - - - - - - - - -" << std::endl;
//    txt << "LISP Based Representation:" << std::endl;
//    txt << KnowledgeBase::toString();
//
//    return txt.str();
//}

} // end namespace model
} // end namespace owlapi
