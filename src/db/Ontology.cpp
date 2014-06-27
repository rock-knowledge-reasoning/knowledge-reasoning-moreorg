#include "Ontology.hpp"
#include "rdf/SopranoDB.hpp"
#include <base/Logging.hpp>

namespace owl_om {
namespace db {

Ontology::Ontology()
    : mSparqlInterface(0)
{}

Ontology::Ptr Ontology::fromFile(const std::string& filename)
{
    Ontology::Ptr ontology(new Ontology());
    ontology->mSparqlInterface = new SopranoDB(filename);
    ontology->refresh();

    return ontology;
}

Ontology::~Ontology()
{
    delete mSparqlInterface;
}

query::Results Ontology::findAll(const Uri& subject, const Uri& predicate, const Uri& object) const
{
    query::Bindings bindings;
    Uri s = query::Utils::identifyPlaceholder(subject, &bindings);
    Uri p = query::Utils::identifyPlaceholder(predicate, &bindings);
    Uri o = query::Utils::identifyPlaceholder(object, &bindings);

    return mSparqlInterface->select_where(s,p,o, bindings);
}

void Ontology::refresh()
{
    using namespace owl_om::db::query;
    // Class list
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::Class());
        ResultsIterator it(results);
        while(it.next())
        {
            subclassOf(it["s"], vocabulary::OWL::Thing());
        }
    }

    // Subclasses
    {
        Results results = findAll("?s", vocabulary::RDFS::subClassOf(), "?o");
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            std::string object = it["o"]; // parent class
            subclassOf(subject, object);
        }
    }

    // Properties
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::RDF::Property());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            throw std::runtime_error("Property '" + subject + "' neither object nor data property");
        }
    }
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::DatatypeProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            dataProperty(subject);
        }
    }
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::ObjectProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            objectProperty(subject);
        }
    }
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
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::InverseFunctionalProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            inverseFunctionalProperty(subject);
        }
    }
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::AnnotationProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            LOG_INFO_S << "Annotation property '" << subject << "' ignored for reasoning"; 
        }
    }

    // Get NamedIndividuals
    {
        LOG_INFO_S << "Searching for NamedIndividuals";
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::NamedIndividual());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
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
        }
    }

    //
    {

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
    return txt;
}

} // end namespace db
} // end namespace owl_om
