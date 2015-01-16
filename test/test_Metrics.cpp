#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/OrganizationModel.hpp>
#include <owl_om/metrics/Redundancy.hpp>
#include <owl_om/owlapi/model/OWLOntologyAsk.hpp>

using namespace owl_om;
using namespace owlapi;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(redundancy_metrics)

BOOST_AUTO_TEST_CASE(it_should_handle_redundancy_metrics)
{
    OrganizationModel om( getRootDir() + "/test/data/om-schema-v0.6.owl" );

    //using namespace owlapi::vocabulary;
    //{
    //    owlapi::model::IRI instance = om.createNewInstance(OM::resolve("Sherpa"), true);
    //    om.createNewInstance(OM::resolve("PayloadCamera"), true);

    //    BOOST_TEST_MESSAGE("Created new from model" << instance);
    //    owlapi::model::OWLOntologyAsk ask(om.ontology());
    //    bool isInstance = ask.isInstanceOf(instance, OM::Actor());
    //    BOOST_REQUIRE_MESSAGE(isInstance, "New model instance of Actor");
    //}

    OWLOntologyTell tell(om.ontology());

    OWLClass::Ptr a = tell.getOWLClass("http://klass/base");
    OWLClass::Ptr b = tell.getOWLClass("http://klass/base-derived");
    OWLClass::Ptr c = tell.getOWLClass("http://klass/base-derived-derived");
    OWLObjectProperty::Ptr property = tell.getOWLObjectProperty("http://property/has");

    tell.subclassOf(c,b);
    tell.subclassOf(b,a);
    om.ontology()->refresh();

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

    metrics::Redundancy redundancy(om);
    double redundancyVal = redundancy.compute(query, resourcePool);
    BOOST_TEST_MESSAGE("Redundancy test: " << redundancyVal); 
}

BOOST_AUTO_TEST_CASE(it_should_handle_metric_map_computation)
{
    OrganizationModel om( getRootDir() + "/test/data/om-schema-v0.6.owl" );
    om.ontology()->refresh();
    metrics::Redundancy redundancy(om);

    metrics::IRISurvivabilityMap survivability = redundancy.compute();

    BOOST_TEST_MESSAGE("Survivability: \n" << metrics::Redundancy::toString(survivability));

    {
        IRI locationImageProvider("http://www.rock-robotics.org/2014/01/om-schema#ImageProvider");
        IRI sherpa("http://www.rock-robotics.org/2014/01/om-schema#Sherpa");

        std::map<IRI, uint32_t> models;
        for(int i = 1; i <= 3; ++i)
        {
            models[sherpa] = i;
            double pSurvivability = redundancy.computeModelBasedProbabilityOfSurvival(locationImageProvider, models);
            BOOST_TEST_MESSAGE("Survivability for #" << i << " " << sherpa << ": \n" << pSurvivability);
        }
    }
    {
        IRI locationImageProvider("http://www.rock-robotics.org/2014/01/om-schema#LocationImageProvider");
        IRI crex("http://www.rock-robotics.org/2014/01/om-schema#CREX");

        std::map<IRI, uint32_t> models;
        for(int i = 1; i <= 3; ++i)
        {
            models[crex] = i;
            double pSurvivability = redundancy.computeModelBasedProbabilityOfSurvival(locationImageProvider, models);
            BOOST_TEST_MESSAGE("Survivability for #" << i << " " << crex << ": \n" << pSurvivability);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
