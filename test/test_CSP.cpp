#include <boost/test/unit_test.hpp>
#include <owl_om/owlapi/csp/ResourceMatch.hpp>
#include <owl_om/owlapi/model/OWLOntologyReader.hpp>
#include <owl_om/owlapi/model/OWLOntologyTell.hpp>
#include <owl_om/owlapi/OWLApi.hpp>
#include <boost/foreach.hpp>

#include "test_utils.hpp"

using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(csp)

//BOOST_AUTO_TEST_CASE(it_should_match_resources)
//{
//    OWLOntologyReader reader;
//    //OWLOntology::Ptr ontology(new OWLOntology());
//    OWLOntology::Ptr ontology = reader.fromFile( getRootDir() + "/test/data/om-schema-v0.6.owl");
//    ontology->refresh();
//
//    OWLOntologyTell tell(ontology);
//    OWLOntologyAsk ask(ontology);
//    tell.initializeDefaultClasses();
//    
//
//    OWLClass::Ptr a = tell.getOWLClass("http://klass/base");
//    OWLClass::Ptr b = tell.getOWLClass("http://klass/base-derived");
//    OWLClass::Ptr c = tell.getOWLClass("http://klass/base-derived-derived");
//
//    tell.subclassOf(c,b);
//    tell.subclassOf(b,a);
//    ontology->refresh();
//    BOOST_REQUIRE_MESSAGE(ask.isSubclassOf(c->getIRI(), b->getIRI()), "C should be subclass of b");
//
//    IRIList query, resourcePool;
//
//    query.push_back(a->getIRI());
//    query.push_back(c->getIRI());
//
//    resourcePool.push_back(b->getIRI());
//    resourcePool.push_back(c->getIRI());
//
//    owlapi::csp::ResourceMatch* match = owlapi::csp::ResourceMatch::solve(query, resourcePool, ontology);
//
//    BOOST_TEST_MESSAGE("Assignment: " << match->toString());
//
//    IRI assignment = match->getAssignment(a->getIRI());
//    BOOST_REQUIRE_MESSAGE( assignment == b->getIRI(), "Expected base to be matched by base-derived");
//}

BOOST_AUTO_TEST_CASE(it_should_match_resource_via_restrictions)
{
    OWLOntologyReader reader;
    OWLOntology::Ptr ontology(new OWLOntology());
    OWLOntologyTell tell(ontology);
    OWLOntologyAsk ask(ontology);
    tell.initializeDefaultClasses();

    OWLClass::Ptr a = tell.getOWLClass("http://klass/base");
    OWLClass::Ptr b = tell.getOWLClass("http://klass/base-derived");
    OWLClass::Ptr c = tell.getOWLClass("http://klass/base-derived-derived");
    OWLObjectProperty::Ptr property = tell.getOWLObjectProperty("http://property/has");

    tell.subclassOf(c,b);
    tell.subclassOf(b,a);
    ontology->refresh();
    BOOST_REQUIRE_MESSAGE(ask.isSubclassOf(c->getIRI(), b->getIRI()), "C should be subclass of b");



    std::vector<OWLCardinalityRestriction::Ptr> query, resourcePool;
    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 2, a->getIRI()));
        query.push_back(restriction);
    }
    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 2, c->getIRI()));
        query.push_back(restriction);
    }

    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 3, b->getIRI()));
        resourcePool.push_back(restriction);
    }
    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 3, c->getIRI()));
        resourcePool.push_back(restriction);
    }

    owlapi::csp::ResourceMatch* match = owlapi::csp::ResourceMatch::solve(query, resourcePool, ontology);

    BOOST_TEST_MESSAGE("Assignment: " << match->toString());
    std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = query.begin();
    for(; cit != query.end(); ++cit)
    {
        BOOST_TEST_MESSAGE("Assignment: " << (*cit)->toString() << " -- " << IRI::toString( match->getAssignedResources(*cit) ) );
    }

    BOOST_TEST_MESSAGE("Unassigned: " << IRI::toString( match->getUnassignedResources() ) );
    delete match;
}

BOOST_AUTO_TEST_CASE(csp_test_provider_via_restrictions)
{
    OWLOntologyReader reader;
    //OWLOntology::Ptr ontology(new OWLOntology());
    OWLOntology::Ptr ontology = reader.fromFile( getRootDir() + "/test/data/om-schema-v0.6.owl");
    ontology->refresh();

    OWLOntologyTell tell(ontology);
    OWLOntologyAsk ask(ontology);
    tell.initializeDefaultClasses();

    IRI sherpa = owlapi::vocabulary::OM::resolve("Sherpa");
    IRI move_to = owlapi::vocabulary::OM::resolve("MoveTo");
    IRI image_provider = owlapi::vocabulary::OM::resolve("ImageProvider");
    IRI stereo_image_provider = owlapi::vocabulary::OM::resolve("StereoImageProvider");
    IRI location_image_provider = owlapi::vocabulary::OM::resolve("LocationImageProvider");

    std::vector<OWLCardinalityRestriction::Ptr> r_sherpa = ask.getCardinalityRestrictions(sherpa);
    std::vector<OWLCardinalityRestriction::Ptr> r_move_to = ask.getCardinalityRestrictions(move_to);
    std::vector<OWLCardinalityRestriction::Ptr> r_image_provider = ask.getCardinalityRestrictions(image_provider);
    std::vector<OWLCardinalityRestriction::Ptr> r_stereo_image_provider = ask.getCardinalityRestrictions(stereo_image_provider);
    std::vector<OWLCardinalityRestriction::Ptr> r_location_image_provider = ask.getCardinalityRestrictions(location_image_provider);

    owlapi::csp::ResourceMatch* fulfillment = owlapi::csp::ResourceMatch::solve(r_move_to, r_sherpa, ontology);
    BOOST_TEST_MESSAGE("Assignment: " << fulfillment->toString());
    delete fulfillment;
    fulfillment = NULL;

    fulfillment = owlapi::csp::ResourceMatch::solve(r_image_provider, r_sherpa, ontology);
    BOOST_TEST_MESSAGE("Assignment: " << fulfillment->toString());
    delete fulfillment;
    fulfillment = NULL;

    {
        OWLObjectProperty::Ptr hasProperty = ask.getOWLObjectProperty( owlapi::vocabulary::OM::resolve("has") );
        OWLCardinalityRestriction::Ptr restriction(new OWLObjectMinCardinality(hasProperty, 1, image_provider));
        tell.subclassOf(sherpa, restriction);
    }

    fulfillment = owlapi::csp::ResourceMatch::solve(r_stereo_image_provider, r_sherpa, ontology);
    BOOST_TEST_MESSAGE("Assignment: " << fulfillment->toString());
    delete fulfillment;
    fulfillment = NULL;

    {
        OWLObjectProperty::Ptr hasProperty = ask.getOWLObjectProperty( owlapi::vocabulary::OM::resolve("has") );
        OWLCardinalityRestriction::Ptr restriction(new OWLObjectMinCardinality(hasProperty, 1, move_to));
        tell.subclassOf(sherpa, restriction);
    }

    // Second level of service provisioning -- requires other services to be
    // recognized: move_to and stereo
    //
    // 1. first infer 1. level services
    //
    // Constraints: 
    //  subclassing constraints, should also allow to account for limiting
    //  facts, e.g. sherpa+crex -> enable, disable, but that will be important
    //  lateron
    //
    //
    //  TODO: add --> infer service -- mark_service "add provides", -->
    //  getCardinalityRestrictions(... , filter on qualification, e.g. 'has','provide')
    //
    //  restriction tree
    //  --> location_image_provider
    //    --> 1 - move_to
    //    --> 1 - image_provider
    std::vector<OWLCardinalityRestriction::Ptr> r_sherpa_with_service = ask.getCardinalityRestrictions(sherpa);
    try {
        fulfillment = owlapi::csp::ResourceMatch::solve(r_location_image_provider, r_sherpa_with_service, ontology);
        BOOST_TEST_MESSAGE("Assignment: " << fulfillment->toString());
    } catch(...)
    {
        BOOST_TEST_MESSAGE("Assignment failed for: " << location_image_provider);
        BOOST_FOREACH(OWLCardinalityRestriction::Ptr r, r_sherpa_with_service)
        {
            BOOST_TEST_MESSAGE("Cardinality: " << r->toString());
        }
    }
    delete fulfillment;
}
    

//    IRI assignment = match->getAssignment(a->getIRI());
//    BOOST_REQUIRE_MESSAGE( assignment == b->getIRI(), "Expected base to be matched by base-derived");

BOOST_AUTO_TEST_SUITE_END()
