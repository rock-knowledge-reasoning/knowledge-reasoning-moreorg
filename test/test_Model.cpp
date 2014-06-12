#include <boost/test/unit_test.hpp>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Registry.hpp>
#include <owl_om/db/KnowledgeBase.hpp>
#include <owl_om/db/rdf/SopranoDB.hpp>
#include "test_utils.hpp"

using namespace owl_om;

std::string rdfTestFiles[] = { "test/data/om-schema-v0.1.owl" };

BOOST_AUTO_TEST_CASE(it_should_query_db)
{
    std::string baseUri = "http://www.rock-robotics.org/2013/09/om-schema#";
    db::SopranoDB db(getRootDir() + rdfTestFiles[0], baseUri);

    db::query::Bindings bindings;
    bindings.push_back("s");
    bindings.push_back("p");
    bindings.push_back("o");

    std::string prefix = "PREFIX rdf:<" + vocabulary::RDF::BaseUri() +">\n";
    prefix += "PREFIX owl: <" + vocabulary::OWL::BaseUri() + ">\n";
    prefix += "PREFIX rdfs: <" + vocabulary::RDFS::BaseUri() + ">\n";

    db::query::Results results = db.query(prefix + " select ?s where { ?s rdf:type owl:Class . }", bindings);

    BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "Results retrieved");
}

BOOST_AUTO_TEST_CASE(it_should_query_db_abstract)
{
    db::KnowledgeBase::Ptr kb = db::KnowledgeBase::fromFile( getRootDir() + rdfTestFiles[0]);

    db::query::Results results = kb->findAll("?s","?p","?o");
    BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "Results retrieved");
    BOOST_TEST_MESSAGE(results.toString());

    results = kb->findAll("?s", vocabulary::RDFS::subClassOf(),"?o");
    BOOST_TEST_MESSAGE(results.toString());

    BOOST_TEST_MESSAGE("Knowledgebase: ");
    BOOST_TEST_MESSAGE(kb->toString());

    //bool isTrue = kb.isTrue(subject,pred,object);
    //bool knownTrue = kb.isKnownTrue(subject,pred,object);
    //bool unknown = kb.isUnknown(subject,pred,object);

}

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    Class::Ptr test(new Class("http://www.rock-robotics.org/2013/09/om-schmema#Test"));

    std::string klassname = test->getName();

    Registry registry;
    registry.registerClass(test);
    Class::Ptr retrievedClass = registry.getClass(klassname);

    BOOST_REQUIRE_MESSAGE( retrievedClass->getName() == test->getName(), "Registered class: expected '" << test->getName() << "' was '" << retrievedClass->getName() << "'");
    BOOST_REQUIRE_MESSAGE( retrievedClass->subclassOf( vocabulary::OWL::Thing() ) , "Registered class subclasses owl::Thing");

    BOOST_TEST_MESSAGE(registry.toString());
}

BOOST_AUTO_TEST_CASE(it_should_create_correct_class_hierarchy)
{
    Class::Ptr klassRoot(new Class("Root"));
    Class::Ptr klassA(new Class("A"));
    Class::Ptr klassB(new Class("B"));

    klassB->addSuperClass(klassA);
    klassA->addSuperClass(klassRoot);

    Registry registry;
    registry.registerClass(klassA);
    registry.registerClass(klassB);
    registry.registerClass(klassRoot);

    BOOST_REQUIRE_MESSAGE( ! klassA->subclassOf(klassA->getName()), "A is not a subclass of A");
    BOOST_REQUIRE_MESSAGE( *klassB <= *klassB, "B is B or subclass of B");
    BOOST_REQUIRE_MESSAGE( klassB->subclassOf(klassA->getName()), "B is subclass of A");
    BOOST_REQUIRE_MESSAGE( *klassB <= *klassA, "B is subclass of A");
    BOOST_REQUIRE_MESSAGE( klassB->subclassOf(klassRoot->getName()), "B is subclass of Root");
    BOOST_REQUIRE_MESSAGE( *klassB <= *klassRoot, "B is subclass of A");
}

BOOST_AUTO_TEST_CASE(it_should_create_knowledge_base_and_class_registry)
{
    using namespace owl_om::db;
    KnowledgeBase::Ptr kb = KnowledgeBase::fromFile( getRootDir() + rdfTestFiles[0]);
    BOOST_TEST_MESSAGE("Knowledgebase: " << kb->toString());
}
