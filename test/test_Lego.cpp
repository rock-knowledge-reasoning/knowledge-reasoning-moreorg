#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owlapi/Vocabulary.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include <organization_model/reasoning/ResourceMatch.hpp>
#include <organization_model/algebra/Connectivity.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <graph_analysis/GraphAnalysis.hpp>
#include <graph_analysis/GraphIO.hpp>

using namespace owlapi;
using namespace owlapi::model;
using namespace organization_model;
using namespace organization_model::reasoning;
using namespace graph_analysis;

BOOST_AUTO_TEST_SUITE(lego)

BOOST_AUTO_TEST_CASE(list)
{
    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-lego-v0.1.owl"));
    OrganizationModelAsk ask(om);

    owlapi::vocabulary::Custom lego("http://www.rock-robotics.org/2017/10/om-lego#");

    ModelPool modelPool;
    modelPool[ lego.resolve("YellowBlock") ] = 10;
    modelPool[ lego.resolve("GreenBlock") ] = 10;
    modelPool[ lego.resolve("RedBlock") ] = 10;
    modelPool[ lego.resolve("BlueBlock") ] = 10;
    modelPool[ lego.resolve("AdapterBlock") ] = 10;
    modelPool[ lego.resolve("Flag") ] = 1;


    size_t epochs = 1;
    size_t minFeasible = 1;
    for(size_t i = 0; i < epochs; ++i)
    {
        BaseGraph::Ptr baseGraph;
        bool feasible = algebra::Connectivity::isFeasible(modelPool, ask, baseGraph, 0, minFeasible, organization_model::vocabulary::OM::resolve("MechanicalInterface") );
        assert(feasible);
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

BOOST_AUTO_TEST_SUITE_END()

