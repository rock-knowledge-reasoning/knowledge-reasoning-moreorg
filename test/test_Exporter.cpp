#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/exporter/PDDLExporter.hpp>
#include <moreorg/vocabularies/OM.hpp>

using namespace moreorg;

BOOST_AUTO_TEST_SUITE(exporter)

BOOST_AUTO_TEST_CASE(pddl)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    ModelPool modelPool;
    modelPool[sherpa] = 2;

    OrganizationModelAsk ask(om, modelPool, false);
    PDDLExporter exporter(ask);
    exporter.saveDomain("/tmp/moreorg-test-domain.pddl");
    exporter.saveProblem("/tmp/moreorg-test-problem.pddl");
}
BOOST_AUTO_TEST_SUITE_END()
