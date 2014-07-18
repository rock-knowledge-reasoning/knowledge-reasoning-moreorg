#include "Ontology.hpp"
#include "db/rdf/SopranoDB.hpp"
#include "db/rdf/Sparql.hpp"
#include <sstream>
#include <base/Logging.hpp>

namespace owl_om {

Ontology::Ontology()
    : mSparqlInterface(0)
{}

Ontology::Ptr Ontology::fromFile(const std::string& filename)
{
    Ontology::Ptr ontology(new Ontology());
    ontology->mSparqlInterface = new db::SopranoDB(filename);
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

void Ontology::refresh()
{
    using namespace owl_om::db::query;

    {
        db::query::Results results = findAll(Subject(),vocabulary::RDF::type(),vocabulary::OWL::Class());
        ResultsIterator it(results);
        while(it.next())
        {
            owlapi::model::IRI subject = it[Subject()];
            subclassOf(subject, vocabulary::OWL::Thing());
        }
    }

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
                } else if ( object == vocabulary::OWL::ObjectProperty())
                {
                    objectProperty(subject);
                } else if ( object == vocabulary::OWL::FunctionalProperty())
                {
                    // delayed handling
                } else if ( object == vocabulary::OWL::InverseFunctionalProperty())
                {
                    inverseFunctionalProperty(subject);
                } else if (object == vocabulary::OWL::TransitiveProperty())
                {
                //    transitiveProperty(object);
                } else if ( object == vocabulary::OWL::AnnotationProperty())
                {
                    LOG_DEBUG_S << "Annotation property '" << subject << "' ignored for reasoning";
                } else if ( object == vocabulary::OWL::Restriction() )
                {
                    LOG_DEBUG_S << "Found restriction";
                }
            } else if(predicate == vocabulary::RDFS::subClassOf())
            {
                subclassOf(subject, object);
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

    IRIList objectProperties = allObjectProperties();
    IRIList::const_iterator cit = objectProperties.begin();
    for(; cit != objectProperties.end(); ++cit)
    {
        IRI relation = *cit;
        Results results = findAll(Subject(), relation, Object());
        LOG_DEBUG_S << "PROPERTY QUERY RESULTS:" << std::endl
            << results.toString() << std::endl;
        ResultsIterator it(results);
        while(it.next())
        {
            owlapi::model::IRI subject = it[Subject()];
            owlapi::model::IRI object = it[Object()];

            LOG_DEBUG_S << subject << " " << relation << " " << object;
            relatedTo(subject, relation, object);
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

} // end namespace owl_om
