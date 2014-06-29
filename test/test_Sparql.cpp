#include <boost/test/unit_test.hpp>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Registry.hpp>
#include <owl_om/Ontology.hpp>
#include <owl_om/db/rdf/SopranoDB.hpp>
#include <owl_om/db/rdf/Sparql.hpp>
#include "test_utils.hpp"

using namespace owl_om;

extern std::string rdfTestFiles[];

BOOST_AUTO_TEST_CASE(it_should_query_db_with_sparql)
{
    std::string baseUri = "http://www.rock-robotics.org/2013/09/om-schema#";
    db::SopranoDB db(getRootDir() + rdfTestFiles[0], baseUri);

    //db::query::Bindings bindings;
    //bindings.push_back("s");
    //bindings.push_back("p");
    //bindings.push_back("o");

    //std::string prefix = "PREFIX rdf:<" + vocabulary::RDF::BaseUri() +">\n";
    //prefix += "PREFIX owl: <" + vocabulary::OWL::BaseUri() + ">\n";
    //prefix += "PREFIX rdfs: <" + vocabulary::RDFS::BaseUri() + ">\n";

    //db::query::Results results = db.query(prefix + " select ?s where { ?s rdf:type owl:Class . }", bindings);
    //BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "Results retrieved");


    { // unsupported query paths
    //    db::query::Bindings bindings;
    //    bindings.push_back("subject");
    //    bindings.push_back("type");

    //    std::string query = "PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\n";
    //    query += "PREFIX owl: <http://www.w3.org/2002/07/owl#>\n";
    //    query += "PREFIX xsd: <http://www.w3.org/2001/XMLSchema#>\n";
    //    query += "PREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#>\n";
    //    query += "PREFIX rock: <http://www.rock-robotics.org/2013/09/om-schema#>\n";
    //    query += " SELECT ?subject ?type \n";
    //    query += " WHERE { ?subject rdf:type/rdfs:subClassOf* ?type }" ;
    //    db::query::Results results = db.query(query, bindings);
    //    BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "Results retrieved count: " << results.rows.size());
    }

    { // unsupported query paths
    //    db::query::Bindings bindings;
    //    bindings.push_back("subject");
    //    bindings.push_back("type");

    //    std::string query = "PREFIX rdf: <http://www.w3.org/1999/02/22-rdf-syntax-ns#>\n";
    //    query += "PREFIX owl: <http://www.w3.org/2002/07/owl#>\n";
    //    query += "PREFIX xsd: <http://www.w3.org/2001/XMLSchema#>\n";
    //    query += "PREFIX rdfs: <http://www.w3.org/2000/01/rdf-schema#>\n";
    //    query += "PREFIX rock: <http://www.rock-robotics.org/2013/09/om-schema#>\n";
    //    query += " SELECT ?subject ?type \n";
    //    query += " WHERE { ?subject rdf:type ?type .\n";
    //    query += "     ?subject !rdf:type owl:Class .\n";
    //    query += " }";
    //    db::query::Results results = db.query(query, bindings);
    //    BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "Filter results retrieved count: " << results.rows.size());
    }

    {
        db::rdf::sparql::Query query;
        query.select("?s").select("?type") \
            .beginWhere() \
                .triple("?s",vocabulary::RDF::type(), "?type") \
                // unsupported with rasqual
                //.minus("?s", vocabulary::RDF::type(), vocabulary::OWL::Class())
            .endWhere();

        BOOST_TEST_MESSAGE("Display query: " << query.toString() );
        db::query::Results results = db.query(query.toString(), query.getBindings());
        BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "Results retrieved count: " << results.rows.size());
    }

    {
        db::rdf::sparql::Query query;
        query.select("?s")
            .beginWhere() \
                .triple("?s",vocabulary::RDF::type(), vocabulary::OWL::NamedIndividual()) \
            .endWhere();

        BOOST_TEST_MESSAGE("Display query: " << query.toString() );
        db::query::Results results = db.query(query.toString(), query.getBindings());
        BOOST_REQUIRE_MESSAGE(results.rows.size() != 0, "NamedIndividuals retrieved count: " << results.rows.size());

        db::query::ResultsIterator it(results);
        while(it.next())
        {
            BOOST_REQUIRE_MESSAGE(!it["?s"].empty(), "Result expected to be not empty: '" << it["s"]);
        }
    }
}
