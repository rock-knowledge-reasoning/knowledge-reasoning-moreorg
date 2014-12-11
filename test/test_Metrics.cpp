#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/OrganizationModel.hpp>
#include <owl_om/metrics/Redundancy.hpp>

using namespace owl_om;

BOOST_AUTO_TEST_CASE(it_should_handle_redundancy_metrics)
{
    OrganizationModel om( getRootDir() + "/test/data/om-schema-v0.5.owl" );

    using namespace owl_om;
    using namespace owl_om::vocabulary;
    {
        IRI instance = om.createNewInstance(OM::resolve("Sherpa"), true);
        om.createNewInstance(OM::resolve("PayloadCamera"), true);

        BOOST_TEST_MESSAGE("Created new from model" << instance);
        BOOST_REQUIRE_MESSAGE( om.ontology()->isInstanceOf(instance, OM::Actor()), "New model instance of Actor");
    }
    om.refresh();

    metrics::Redundancy redundancy(om);
    metrics::IRIMetricMap metrics = redundancy.compute();
}
