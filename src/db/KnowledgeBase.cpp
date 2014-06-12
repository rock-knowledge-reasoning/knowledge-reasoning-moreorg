#include "KnowledgeBase.hpp"
#include "rdf/SopranoDB.hpp"
#include <base/Logging.hpp>

namespace owl_om {
namespace db {

KnowledgeBase::KnowledgeBase()
    : mSparqlInterface(0)
{}

KnowledgeBase::Ptr KnowledgeBase::fromFile(const std::string& filename)
{
    KnowledgeBase::Ptr knowledgeBase(new KnowledgeBase());
    knowledgeBase->mSparqlInterface = new SopranoDB(filename);

    knowledgeBase->updateRegistry(&knowledgeBase->mRegistry);
    knowledgeBase->updateInstances(&knowledgeBase->mInstances);

    return knowledgeBase;
}

KnowledgeBase::~KnowledgeBase()
{
    delete mSparqlInterface;
}

query::Results KnowledgeBase::findAll(const Uri& subject, const Uri& predicate, const Uri& object) const
{
    query::Bindings bindings;
    Uri s = query::Utils::identifyPlaceholder(subject, &bindings);
    Uri p = query::Utils::identifyPlaceholder(predicate, &bindings);
    Uri o = query::Utils::identifyPlaceholder(object, &bindings);

    return mSparqlInterface->select_where(s,p,o, bindings);
}

void KnowledgeBase::updateRegistry(Registry* registry) const
{
    using namespace owl_om::db::query;
    // Class list
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::Class());
        ResultsIterator it(results);
        while(it.next())
        {
            Class::Ptr klass(new Class( it["s"] ));
            registry->registerClass(klass);
        }
    }

    // Subclasses
    {
        Results results = findAll("?s", vocabulary::RDFS::subClassOf(), "?o");
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            std::string object = it["o"];

            Class::Ptr subjectKlass = registry->getClass(subject, true /* lazy */);
            Class::Ptr objectKlass = registry->getClass(object, true /* lazy */);
            subjectKlass->addSuperClass(objectKlass);
        }
    }

    // Properties
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::RDF::Property());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];

            Property::Ptr property(new Property(subject) );
            registry->registerProperty(property);
        }
    }
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::DatatypeProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];

            Property::Ptr property(new DatatypeProperty(subject));
            registry->registerProperty(property);
        }
    }
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::ObjectProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];

            Property::Ptr property(new ObjectProperty(subject) );
            registry->registerProperty(property);
        }
    }
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::AnnotationProperty());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];

            Property::Ptr property(new AnnotationProperty(subject) );
            registry->registerProperty(property);
        }
    }

}

void KnowledgeBase::updateInstances(Instance::List* instances)
{
    using namespace query;
    // Get NamedIndividuals
    {
        Results results = findAll("?s", vocabulary::RDF::type(), vocabulary::OWL::NamedIndividual());
        ResultsIterator it(results);
        while(it.next())
        {
            std::string subject = it["s"];
            Results objects = findAll(subject, vocabulary::RDF::type(), "?o");
            ResultsIterator objectsIt(objects);
            while(objectsIt.next())
            {
                Uri classType = objectsIt["o"];
                if(classType != vocabulary::OWL::NamedIndividual())
                {
                    Instance::Ptr instance(new Instance(subject, mRegistry.getClass(classType) ));
                    instances->push_back(instance);
                }
            }
        }
    }
}

std::string KnowledgeBase::toString() const
{
    std::string txt = "KnowledgeBase:\n";
    txt += mRegistry.toString(); 
    txt += "\n\n";
    txt += "Instances (ABox statements):\n";

    Instance::List::const_iterator cit = mInstances.begin();
    for(; cit != mInstances.end(); ++cit)
    {
        Instance::Ptr instance = *cit;
        txt += instance->toString() + "\n";
    }
    return txt;
}

} // end namespace db
} // end namespace owl_om
