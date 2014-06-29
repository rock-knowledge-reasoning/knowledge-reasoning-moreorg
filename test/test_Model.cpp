#include <boost/test/unit_test.hpp>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Registry.hpp>
#include <owl_om/Ontology.hpp>
#include <owl_om/db/rdf/SopranoDB.hpp>
#include "test_utils.hpp"

using namespace owl_om;

std::string rdfTestFiles[] = { "test/data/om-schema-v0.1.owl", "test/data/om-schema-v0.2.owl" };

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
    Ontology::Ptr ontology = Ontology::fromFile( getRootDir() + rdfTestFiles[0]);

    db::query::Results results = ontology->findAll("?s","?p","?o");
    BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "Results retrieved");
    BOOST_TEST_MESSAGE(results.toString());

    results = ontology->findAll("?s", vocabulary::RDFS::subClassOf(),"?o");
    BOOST_TEST_MESSAGE(results.toString());

    BOOST_TEST_MESSAGE("Ontology: ");
    BOOST_TEST_MESSAGE(ontology->toString());
}

BOOST_AUTO_TEST_CASE(it_should_create_ontology)
{
    using namespace owl_om;
    Ontology::Ptr ontology = Ontology::fromFile( getRootDir() + rdfTestFiles[0]);
    BOOST_TEST_MESSAGE("Ontology: " << ontology->toString());
}
