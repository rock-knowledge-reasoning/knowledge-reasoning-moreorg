#include <boost/test/unit_test.hpp>
#include <organization_model/exporter/PDDLExporter.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"

using namespace organization_model;

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
    exporter.saveDomain("/tmp/organization_model-test-domain.pddl");
    exporter.saveProblem("/tmp/organization_model-test-problem.pddl");

}
BOOST_AUTO_TEST_SUITE_END()
