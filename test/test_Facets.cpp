#include <boost/test/unit_test.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/facets/Robot.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"
#include <organization_model/vocabularies/Robot.hpp>

using namespace organization_model;

BOOST_AUTO_TEST_SUITE(facets)

BOOST_AUTO_TEST_CASE(robot)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace organization_model::vocabulary;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
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
            owlapi::model::IRI f = vocabulary::OM::resolve("LogisticHubProvider");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(!supporting, "Robot is not supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("TransportProvider");
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

    IRI organizationModelIRI = "http://www.rock-robotics.org/2015/12/projects/TransTerrA";
    OrganizationModel::Ptr om(new OrganizationModel(organizationModelIRI));

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
            owlapi::model::IRI f = vocabulary::OM::resolve("LogisticHubProvider");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(!supporting, "Robot is not supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("TransportProvider");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(supporting, "Robot is supporting '" << f << "'");
        }
    }

    {
        organization_model::facets::Robot robot(sherpa, ask);
        uint32_t transportDemand = robot.getTransportDemand();
        BOOST_REQUIRE_MESSAGE( transportDemand == 1, "Robot " << sherpa << " has transport demand of: " << transportDemand);

        uint32_t transportCapacity = robot.getTransportCapacity();
        BOOST_REQUIRE_MESSAGE( transportCapacity == 10, "Robot " << sherpa << " has transport capacity of: " << transportCapacity);

        {
            IRI model = vocabulary::OM::resolve("CREX");
            uint32_t transportCapacityForModel = robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE( transportCapacityForModel == 2, "Robot " << sherpa << " has transport capacity of " << transportCapacityForModel << " for " << model);
        }

        {
            IRI model = vocabulary::OM::resolve("CoyoteIII");
            uint32_t transportCapacityForModel = robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE( transportCapacityForModel == 2, "Robot " << sherpa << " has transport capacity of " << transportCapacityForModel << " for " << model);
        }

        {
            IRI model = vocabulary::OM::resolve("BaseCamp");
            uint32_t transportCapacityForModel = robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE( transportCapacityForModel == 2, "Robot " << sherpa << " has transport capacity of " << transportCapacityForModel << " for " << model);
        }

        {
            IRI model = vocabulary::OM::resolve("Payload");
            uint32_t transportCapacityForModel = robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE( transportCapacityForModel == 10, "Robot " << sherpa << " has transport capacity of " << transportCapacityForModel << " for " << model);
        }

    }
    {
        organization_model::facets::Robot robot(basecamp, ask);
        uint32_t transportDemand = robot.getTransportDemand();
        BOOST_REQUIRE_MESSAGE( transportDemand == 1, "Robot " << basecamp << " has transport demand of: " << transportDemand);
    }

    {
        organization_model::facets::Robot robot(payload, ask);
        uint32_t transportDemand = robot.getTransportDemand();
        BOOST_REQUIRE_MESSAGE( transportDemand == 1, "Robot " << payload << " has transport demand of: " << transportDemand);
    }
}

BOOST_AUTO_TEST_CASE(composite_robot_from_transterra)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace organization_model::vocabulary;

    IRI organizationModelIRI = "http://www.rock-robotics.org/2015/12/projects/TransTerrA";
    OrganizationModel::Ptr om(new OrganizationModel(organizationModelIRI));

    IRI sherpa = OM::resolve("Sherpa");
    IRI payload = OM::resolve("Payload");
    IRI basecamp = OM::resolve("BaseCamp");

    ModelPool modelPool;
    modelPool[sherpa] = 3;
    modelPool[payload] = 3;
    modelPool[basecamp] = 1;

    OrganizationModelAsk ask(om, modelPool, true);
    BOOST_TEST_MESSAGE("Functionality mapping: " << ask.getFunctionalityMapping().toString(4));
    {
        organization_model::facets::Robot robot(modelPool, ask);
        BOOST_REQUIRE_MESSAGE(robot.isMobile(), "Robot " << modelPool.toString() << " is mobile");
    }
}

BOOST_AUTO_TEST_SUITE_END()
