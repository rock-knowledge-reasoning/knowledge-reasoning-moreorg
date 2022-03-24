#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/facades/Robot.hpp>
#include <moreorg/policies/DistributionPolicy.hpp>
#include <moreorg/policies/SelectionPolicy.hpp>
#include <moreorg/vocabularies/OM.hpp>
#include <moreorg/vocabularies/Robot.hpp>

using namespace moreorg;

BOOST_AUTO_TEST_SUITE(facades)

BOOST_AUTO_TEST_CASE(robot)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace moreorg::vocabulary;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = OM::resolve("Sherpa");
    IRI payload = OM::resolve("Payload");

    ModelPool modelPool;
    modelPool[sherpa] = 3;
    modelPool[payload] = 3;

    OrganizationModelAsk ask(om, modelPool, true);
    {
        moreorg::facades::Robot robot(sherpa, ask);
        BOOST_REQUIRE_MESSAGE(robot.isMobile(),
                              "Robot " << sherpa << " is mobile");
        BOOST_REQUIRE_MESSAGE(
            robot.getPropertyValue(vocabulary::OM::mass()) == robot.getMass(),
            "Mass via numeric value should be equal to the robot mass");
    }
    {
        moreorg::facades::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(),
                              "Robot " << payload << " is not mobile");
        BOOST_REQUIRE_MESSAGE(robot.getTransportDemand() != 0,
                              "Robot " << payload
                                       << " has transport demand > 0");
    }
    {
        moreorg::facades::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(),
                              "Robot " << payload << " is not mobile");
    }
    {
        OrganizationModelAsk ask(om, modelPool, true);
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("Capability");
            BOOST_REQUIRE_THROW(ask.isSupporting(sherpa, f),
                                std::runtime_error);
        }
        {
            owlapi::model::IRI f =
                vocabulary::OM::resolve("LogisticHubProvider");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(!supporting,
                                  "Robot is not supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("TransportProvider");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(supporting,
                                  "Robot is supporting '" << f << "'");
        }
        {
            moreorg::facades::Robot robot(modelPool, ask);
            BOOST_REQUIRE_MESSAGE(
                robot.getPropertyValue(vocabulary::OM::mass()) ==
                    robot.getMass(),
                "Composite agent: Mass via numeric value should be equal to "
                "the robot mass");
        }
    }
}

BOOST_AUTO_TEST_CASE(robot_from_transterra)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace moreorg::vocabulary;

    IRI organizationModelIRI =
        "http://www.rock-robotics.org/2015/12/projects/TransTerrA";
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
        moreorg::facades::Robot robot(sherpa, ask);
        BOOST_REQUIRE_MESSAGE(robot.isMobile(),
                              "Robot " << sherpa << " is mobile");
    }
    {
        moreorg::facades::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(),
                              "Robot " << payload << " is not mobile");
    }
    {
        moreorg::facades::Robot robot(payload, ask);
        BOOST_REQUIRE_MESSAGE(!robot.isMobile(),
                              "Robot " << payload << " is not mobile");
    }
    {
        OrganizationModelAsk ask(om, modelPool, true);
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("Capability");
            BOOST_REQUIRE_THROW(ask.isSupporting(sherpa, f),
                                std::runtime_error);
        }
        {
            owlapi::model::IRI f =
                vocabulary::OM::resolve("LogisticHubProvider");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(!supporting,
                                  "Robot is not supporting '" << f << "'");
        }
        {
            owlapi::model::IRI f = vocabulary::OM::resolve("TransportProvider");
            bool supporting = ask.isSupporting(sherpa, f);
            BOOST_TEST_MESSAGE(ask.toString());
            BOOST_REQUIRE_MESSAGE(supporting,
                                  "Robot is supporting '" << f << "'");
        }
    }

    {
        moreorg::facades::Robot robot(sherpa, ask);
        uint32_t transportDemand = robot.getTransportDemand();
        BOOST_REQUIRE_MESSAGE(transportDemand == 1,
                              "Robot " << sherpa << " has transport demand of: "
                                       << transportDemand);

        uint32_t transportCapacity = robot.getTransportCapacity();
        BOOST_REQUIRE_MESSAGE(transportCapacity == 10,
                              "Robot " << sherpa
                                       << " has transport capacity of: "
                                       << transportCapacity);

        policies::Selection selection =
            robot.getSelection(vocabulary::OM::TransportProviderSelection());
        BOOST_REQUIRE_MESSAGE(
            selection.size() == 1,
            "Robot " << sherpa
                     << " is transport provider, selection is: " << selection);

        // policies::Distribution distribution =
        // robot.getDistribution(vocabulary::OM::DistributionPolicy_EnergyProvider());
        // BOOST_REQUIRE_MESSAGE(distribution.shares[sherpa] == 1, "Robot " <<
        //        sherpa << " is energy provider");

        {
            IRI model = vocabulary::OM::resolve("CREX");
            uint32_t transportCapacityForModel =
                robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE(
                transportCapacityForModel == 2,
                "Robot " << sherpa << " has transport capacity of "
                         << transportCapacityForModel << " for " << model);
        }

        {
            IRI model = vocabulary::OM::resolve("CoyoteIII");
            uint32_t transportCapacityForModel =
                robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE(
                transportCapacityForModel == 2,
                "Robot " << sherpa << " has transport capacity of "
                         << transportCapacityForModel << " for " << model);
        }

        {
            IRI model = vocabulary::OM::resolve("BaseCamp");
            uint32_t transportCapacityForModel =
                robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE(
                transportCapacityForModel == 2,
                "Robot " << sherpa << " has transport capacity of "
                         << transportCapacityForModel << " for " << model);
        }

        {
            IRI model = vocabulary::OM::resolve("Payload");
            uint32_t transportCapacityForModel =
                robot.getTransportCapacity(model);
            BOOST_REQUIRE_MESSAGE(
                transportCapacityForModel == 10,
                "Robot " << sherpa << " has transport capacity of "
                         << transportCapacityForModel << " for " << model);
        }
    }
    {
        moreorg::facades::Robot robot(basecamp, ask);
        uint32_t transportDemand = robot.getTransportDemand();
        BOOST_REQUIRE_MESSAGE(transportDemand == 1,
                              "Robot " << basecamp
                                       << " has transport demand of: "
                                       << transportDemand);
    }

    {
        moreorg::facades::Robot robot(payload, ask);
        uint32_t transportDemand = robot.getTransportDemand();
        BOOST_REQUIRE_MESSAGE(transportDemand == 1,
                              "Robot " << payload
                                       << " has transport demand of: "
                                       << transportDemand);

        policies::Selection selection =
            robot.getSelection(vocabulary::OM::TransportProviderSelection());
        BOOST_REQUIRE_MESSAGE(selection.empty(),
                              "Robot has no transport"
                              " provider: '"
                                  << selection << "' " << selection.size());
    }
}

BOOST_AUTO_TEST_CASE(composite_robot_from_transterra)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace moreorg::vocabulary;

    IRI organizationModelIRI =
        "http://www.rock-robotics.org/2015/12/projects/TransTerrA";
    OrganizationModel::Ptr om =
        make_shared<OrganizationModel>(organizationModelIRI);

    IRI sherpa = OM::resolve("Sherpa");
    IRI payload = OM::resolve("Payload");
    IRI basecamp = OM::resolve("BaseCamp");

    ModelPool modelPool;
    modelPool[sherpa] = 3;
    modelPool[payload] = 3;
    modelPool[basecamp] = 1;

    OrganizationModelAsk ask(om, modelPool, true);
    BOOST_TEST_MESSAGE(
        "Functionality mapping: " << ask.getFunctionalityMapping().toString(4));
    {
        moreorg::facades::Robot robot(modelPool, ask);
        BOOST_REQUIRE_MESSAGE(robot.isMobile(),
                              "Robot " << modelPool.toString() << " is mobile");

        //        uint32_t transportDemand = robot.getTransportDemand();
        //        BOOST_REQUIRE_MESSAGE( transportDemand == 7, "Robot " <<
        //        modelPool.toString() << " has transport demand of: " <<
        //        transportDemand);
        //
        //        uint32_t transportCapacity = robot.getTransportCapacity();
        //        BOOST_REQUIRE_MESSAGE( transportCapacity == 23, "Robot " <<
        //        modelPool.toString() << " has transport capacity of: " <<
        //        transportCapacity);
    }
}

BOOST_AUTO_TEST_CASE(derived_property)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace moreorg::vocabulary;

    IRI organizationModelIRI =
        "http://www.rock-robotics.org/2015/12/projects/TransTerrA";
    OrganizationModel::Ptr om =
        make_shared<OrganizationModel>(organizationModelIRI);

    IRI sherpa = OM::resolve("Sherpa");
    ModelPool modelPool;
    modelPool[sherpa] = 1;

    OrganizationModelAsk ask(om, modelPool, true);
    {
        moreorg::facades::Robot robot(modelPool, ask);
        double energyCapacity =
            robot.getDataPropertyValue(vocabulary::OM::energyCapacity());

        BOOST_REQUIRE_MESSAGE(
            energyCapacity > 0,
            "Energy Capacity has been derived from other properties: "
                << energyCapacity);
    }
}

BOOST_AUTO_TEST_SUITE_END()
