#include <boost/test/unit_test.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/facets/Robot.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"

using namespace organization_model;

BOOST_AUTO_TEST_SUITE(facets)

BOOST_AUTO_TEST_CASE(robot)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace organization_model::vocabulary;

    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-schema-v0.14.owl"));
    IRI sherpa = OM::resolve("Sherpa");
    IRI payload = OM::resolve("Payload");

    ModelPool modelPool;
    modelPool[sherpa] = 3;
    modelPool[payload] = 3;

    OrganizationModelAsk ask(om, modelPool, true);
    {
        organization_model::facets::Robot robot(sherpa, ask);
        BOOST_REQUIRE_MESSAGE(robot.isMobile(), "Robot " << sherpa << " is mobile");
    }
    {
        organization_model::facets::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(), "Robot " << payload << " is not mobile");
    }
    {
        organization_model::facets::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(), "Robot " << payload << " is not mobile");
    }
    {
        OrganizationModelAsk ask(om,modelPool,true);
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("Capability");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(supporting, "Robot is supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("LogisticHub");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(!supporting, "Robot is not supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("TransportService");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(supporting, "Robot is supporting '" << f << "'");
        }
    }
}

BOOST_AUTO_TEST_CASE(robot_from_transterra)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace organization_model::vocabulary;

    BOOST_TEST_MESSAGE("STARTING TO READ");
    IRI organizationModelIRI = "http://www.rock-robotics.org/2015/12/projects/TransTerrA";
    OrganizationModel::Ptr om(new OrganizationModel(organizationModelIRI));
    BOOST_TEST_MESSAGE("FINISHED READING");
    IRI sherpa = OM::resolve("Sherpa");
    IRI payload = OM::resolve("Payload");
    IRI basecamp = OM::resolve("BaseCamp");

    ModelPool modelPool;
    modelPool[sherpa] = 3;
    modelPool[payload] = 3;
    modelPool[basecamp] = 1;

    OrganizationModelAsk ask(om, modelPool, true);
    {
        organization_model::facets::Robot robot(sherpa, ask);
        BOOST_REQUIRE_MESSAGE(robot.isMobile(), "Robot " << sherpa << " is mobile");
    }
    {
        organization_model::facets::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(), "Robot " << payload << " is not mobile");
    }
    {
        organization_model::facets::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(), "Robot " << payload << " is not mobile");
    }
    {
        OrganizationModelAsk ask(om,modelPool,true);
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("Capability");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(supporting, "Robot is supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("LogisticHub");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(!supporting, "Robot is not supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("TransportService");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(supporting, "Robot is supporting '" << f << "'");
        }
    }

    {
        organization_model::facets::Robot robot(sherpa, ask);
        int32_t transportSupplyDemand = robot.getPayloadTransportSupplyDemand();
        BOOST_REQUIRE_MESSAGE( transportSupplyDemand > 0, "Robot " << sherpa << " has transport supply demand of: " << transportSupplyDemand);

    }
    {
        organization_model::facets::Robot robot(basecamp, ask);
        int32_t transportSupplyDemand = robot.getPayloadTransportSupplyDemand();
        BOOST_REQUIRE_MESSAGE( transportSupplyDemand < 0, "Robot " << basecamp << " has transport supply demand of: " << transportSupplyDemand);
    }

    {
        organization_model::facets::Robot robot(payload, ask);
        int32_t transportSupplyDemand = robot.getPayloadTransportSupplyDemand();
        BOOST_REQUIRE_MESSAGE( transportSupplyDemand < 0, "Robot " << payload << " has transport supply demand of: " << transportSupplyDemand);

    }
}

BOOST_AUTO_TEST_SUITE_END()
