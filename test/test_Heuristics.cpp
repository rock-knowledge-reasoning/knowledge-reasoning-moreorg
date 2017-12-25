#include <boost/test/unit_test.hpp>
#include <organization_model/Heuristics.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"

using namespace organization_model;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(heuristics)

BOOST_AUTO_TEST_CASE(estimate_current_position)
{
    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = vocabulary::OM::resolve("Sherpa");

    ModelPool modelPool;
    modelPool[sherpa] = 1;

    OrganizationModelAsk ask(om, modelPool, true);

    Heuristics heuristics(ask);
    base::Position fromPosition(0.0,0.0,0.0);
    base::Position toPosition(100.0,0.0,0.0);
    size_t durationInS = 60;
    // expecting 0.5 m/s for Sherpa
    double expectedDistance = 60*0.5;

    AtomicAgent atomicAgent(0, sherpa);
    Agent agent(atomicAgent);

    base::Position position = heuristics.positionLinear(agent,
            fromPosition,
            toPosition,
            durationInS);

    BOOST_REQUIRE_MESSAGE(position.x() != expectedDistance && position.y() == 0 && position.z() == 0,
            "Position estimate: " << position.x() << "/" << position.y() << "/" << position.z());
}

BOOST_AUTO_TEST_SUITE_END()
