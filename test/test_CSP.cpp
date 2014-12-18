#include <boost/test/unit_test.hpp>
#include <owl_om/owlapi/csp/ResourceMatching.hpp>
#include <owl_om/owlapi/model/OWLOntologyReader.hpp>
#include <owl_om/owlapi/model/OWLOntologyTell.hpp>
#include <owl_om/owlapi/OWLApi.hpp>
#include "test_utils.hpp"

using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(csp)

BOOST_AUTO_TEST_CASE(it_should_match_resources)
{
    OWLOntologyReader reader;
    //OWLOntology::Ptr ontology(new OWLOntology());
    OWLOntology::Ptr ontology = reader.fromFile( getRootDir() + "/test/data/om-schema-v0.6.owl");
    ontology->refresh();

    OWLOntologyTell tell(ontology);
    OWLOntologyAsk ask(ontology);
    tell.initializeDefaultClasses();
    

    OWLClass::Ptr a = tell.getOWLClass("http://klass/base");
    OWLClass::Ptr b = tell.getOWLClass("http://klass/base-derived");
    OWLClass::Ptr c = tell.getOWLClass("http://klass/base-derived-derived");

    tell.subclassOf(c,b);
    tell.subclassOf(b,a);
    ontology->refresh();
    BOOST_REQUIRE_MESSAGE(ask.isSubclassOf(c->getIRI(), b->getIRI()), "C should be subclass of b");

    IRIList query, resourcePool;

    query.push_back(a->getIRI());
    query.push_back(c->getIRI());

    resourcePool.push_back(b->getIRI());
    resourcePool.push_back(c->getIRI());

    owlapi::csp::ResourceMatching* matching = owlapi::csp::ResourceMatching::solve(query, resourcePool, ontology);

    BOOST_TEST_MESSAGE("Assignment: " << matching->toString());
}

BOOST_AUTO_TEST_SUITE_END()
