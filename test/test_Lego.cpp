#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>

#include <graph_analysis/GraphAnalysis.hpp>
#include <graph_analysis/GraphIO.hpp>
#include <moreorg/OrganizationModel.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/algebra/Connectivity.hpp>
#include <moreorg/reasoning/ResourceMatch.hpp>
#include <moreorg/vocabularies/OM.hpp>
#include <owlapi/Vocabulary.hpp>

using namespace owlapi;
using namespace owlapi::model;
using namespace moreorg;
using namespace moreorg::reasoning;
using namespace graph_analysis;

BOOST_AUTO_TEST_SUITE(lego)

BOOST_AUTO_TEST_CASE(list)
{
    OrganizationModel::Ptr om(
        new OrganizationModel(getRootDir() + "/test/data/om-lego-v0.1.owl"));
    OrganizationModelAsk ask(om);

    owlapi::vocabulary::Custom lego(
        "http://www.rock-robotics.org/2017/10/om-lego#");

    // report
    // Statistics:
    // # graph completeness evaluations: 145
    // time in s: 119.324
    // stopped: 0
    // # propagator executions: 3032041
    // # failed nodes: 0
    // # expanded nodes: 7540
    // depth of search stack: 51
    // # restarts: 144
    // # nogoods: 144
    ModelPool modelPool;
    modelPool[lego.resolve("YellowBlock")] = 10;
    modelPool[lego.resolve("GreenBlock")] = 10;
    modelPool[lego.resolve("RedBlock")] = 10;
    modelPool[lego.resolve("BlueBlock")] = 10;
    modelPool[lego.resolve("AdapterBlock")] = 10;
    modelPool[lego.resolve("Flag")] = 1;

    BOOST_TEST_MESSAGE(
        "Please remain patient -- this test can take up to 2-3 min");
    size_t epochs = 1;
    size_t minFeasible = 1;
    for(size_t i = 0; i < epochs; ++i)
    {
        BaseGraph::Ptr baseGraph;
        bool feasible = algebra::Connectivity::isFeasible(
            modelPool,
            ask,
            baseGraph,
            0,
            minFeasible,
            moreorg::vocabulary::OM::resolve("MechanicalInterface"));
        BOOST_REQUIRE_MESSAGE(feasible,
                              "Connectivity check should validate feasibility");
        if(baseGraph)
        {
            std::stringstream ss;
            ss << "/tmp/organization-model-lego-";
            ss << i;
            ss << ".dot";

            graph_analysis::io::GraphIO::write(ss.str(), baseGraph);
        }
        BOOST_TEST_MESSAGE(algebra::Connectivity::getStatistics().toString());
        usleep(200);
    }
}

BOOST_AUTO_TEST_CASE(icaps)
{
    OrganizationModel::Ptr om(
        new OrganizationModel(getRootDir() + "/test/data/om-icaps-v0.1.owl"));
    OrganizationModelAsk ask(om);

    owlapi::vocabulary::Custom lego(
        "http://www.rock-robotics.org/2017/10/om-icaps#");

    ModelPool modelPool;
    modelPool[lego.resolve("YellowAgent")] = 4;
    modelPool[lego.resolve("GreenAgent")] = 3;
    modelPool[lego.resolve("RedAgent")] = 2;
    modelPool[lego.resolve("BlueAgent")] = 1;
    modelPool[lego.resolve("Adapter")] = 1;
    modelPool[lego.resolve("Manipulator")] = 1;

    size_t epochs = 1;
    size_t minFeasible = 1;
    for(size_t i = 0; i < epochs; ++i)
    {
        BaseGraph::Ptr baseGraph;
        bool feasible = algebra::Connectivity::isFeasible(
            modelPool,
            ask,
            baseGraph,
            0,
            minFeasible,
            moreorg::vocabulary::OM::resolve("MechanicalInterface"));
        BOOST_REQUIRE_MESSAGE(feasible,
                              "Connectivity check should validate feasibility");
        if(baseGraph)
        {
            std::stringstream ss;
            ss << "/tmp/organization-model-icaps-";
            ss << i;
            ss << ".dot";

            graph_analysis::io::GraphIO::write(ss.str(), baseGraph);
        }
        BOOST_TEST_MESSAGE(algebra::Connectivity::getStatistics().toString());
        usleep(200);
    }
}

BOOST_AUTO_TEST_SUITE_END()
