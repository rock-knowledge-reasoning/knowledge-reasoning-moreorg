#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/OrganizationModel.hpp>
#include <owl_om/metrics/Redundancy.hpp>
#include <owl_om/owlapi/model/OWLOntologyAsk.hpp>

using namespace owl_om;
using namespace owlapi;

BOOST_AUTO_TEST_CASE(it_should_handle_redundancy_metrics)
{
    OrganizationModel om( getRootDir() + "/test/data/om-schema-v0.5.owl" );

    using namespace owlapi::vocabulary;
    {
        owlapi::model::IRI instance = om.createNewInstance(OM::resolve("Sherpa"), true);
        om.createNewInstance(OM::resolve("PayloadCamera"), true);

        BOOST_TEST_MESSAGE("Created new from model" << instance);
        owlapi::model::OWLOntologyAsk ask(om.ontology());
        bool isInstance = ask.isInstanceOf(instance, OM::Actor());
        BOOST_REQUIRE_MESSAGE(isInstance, "New model instance of Actor");
    }
    om.refresh();

    metrics::Redundancy redundancy(om);
    metrics::IRIMetricMap metrics = redundancy.compute();
}
