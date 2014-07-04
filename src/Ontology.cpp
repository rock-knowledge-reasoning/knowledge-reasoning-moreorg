#include "Ontology.hpp"
#include "db/rdf/SopranoDB.hpp"
#include <base/Logging.hpp>
#include "db/rdf/Sparql.hpp"

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

db::query::Results Ontology::findAll(const Uri& subject, const Uri& predicate, const Uri& object) const
{
    db::query::Results results = mSparqlInterface->findAll(subject, predicate, object);
    LOG_DEBUG_S << "Results: " << results.toString();
    return results;
}

void Ontology::refresh()
{
    using namespace owl_om::db::query;

    {
        db::query::Results results = findAll("?s",vocabulary::RDF::type(),vocabulary::OWL::Class());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            subclassOf(subject, vocabulary::OWL::Thing());
        }
    }

    {
        db::query::Results results = findAll("?s","?p","?o");
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            std::string predicate = it["p"];
            std::string object = it["o"];

            if(predicate == vocabulary::RDF::type())
            {
                // Creating classes
                if(object == vocabulary::OWL::Class())
                {
                    // already registered
                } else if( object == vocabulary::OWL::NamedIndividual())
                {
                    // search for class types, but exclude NamedIndividual 'class'
                    Results objects = findAll(subject, vocabulary::RDF::type(), "?o");
                    ResultsIterator objectsIt(objects);
                    while(objectsIt.next())
                    {
                        Uri classType = objectsIt["o"];
                        if(classType != vocabulary::OWL::NamedIndividual())
                        {
                            LOG_INFO_S << "Registering:" << classType;
                            instanceOf(subject, classType);
                        } else {
                            LOG_INFO_S << "Skipping NamedIndividual " << classType;
                        }
                    }
                } else if (object == vocabulary::RDF::Property())
                {
                    throw std::runtime_error("Property '" + subject + "' neither object nor data property");
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
                    LOG_INFO_S << "Annotation property '" << subject << "' ignored for reasoning"; 
                } else if ( object == vocabulary::OWL::Restriction() )
                {
                    LOG_INFO_S << "Found restriction";
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
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::FunctionalProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            if( isSubclassOf(subject, vocabulary::OWL::ObjectProperty()))
            {
                functionalProperty(subject, OBJECT);
            } else if ( isSubclassOf(subject, vocabulary::OWL::DatatypeProperty()) )
            {
                functionalProperty(subject, DATA);
            } else {
                throw std::invalid_argument("Property '" + subject + "' is not a known object or data property");
            }
        }
    }

    IRIList objectProperties = allObjectProperties();
    IRIList::const_iterator cit = objectProperties.begin();
    for(; cit != objectProperties.end(); ++cit)
    {
        IRI relation = *cit;
        Results results = findAll("?s", relation, "?o");
        LOG_DEBUG_S << "PROPERTY QUERY RESULTS";
        LOG_DEBUG_S << "RESULTS: " << results.toString();
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            std::string object = it["o"];

            LOG_DEBUG_S << subject << " " << relation << " " << object;
            relatedTo(subject, relation, object);
        }
    }
}

std::string Ontology::toString() const
{
    std::string txt = "Ontology:\n";
    txt += "Classes (TBox statements):\n";
    {
        IRIList classes = allClasses();
        IRIList::const_iterator cit = classes.begin();
        for(; cit != classes.end(); ++cit)
        {
            txt += *cit + "\n";
        }
        txt += "\n\n";
    }
    txt += "- - - - - - - - - - - - - - - -\n";
    txt += "Instances (ABox statements):\n";
    {
        IRIList instances = allInstances();
        IRIList::const_iterator cit = instances.begin();
        for(; cit != instances.end(); ++cit)
        {
            txt += *cit + "\n";
        }
    }
    txt += "- - - - - - - - - - - - - - - -\n";
    txt += "ObjectProperties:\n";
    {
        IRIList properties = allObjectProperties();
        IRIList::const_iterator cit = properties.begin();
        for(; cit != properties.end(); ++cit)
        {
            txt += *cit + "\n";
        }
    }
    txt += "- - - - - - - - - - - - - - - -\n";
    txt += "DataProperties:\n";
    {
        IRIList properties = allDataProperties();
        IRIList::const_iterator cit = properties.begin();
        for(; cit != properties.end(); ++cit)
        {
            txt += *cit + "\n";
        }
    }

    txt += "- - - - - - - - - - - - - - - -\n";
    txt += "LISP Based Representation: \n";
    txt += KnowledgeBase::toString();

    return txt;
}

} // end namespace owl_om
