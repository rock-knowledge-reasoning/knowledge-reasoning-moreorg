#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <moreorg/policies/DistributionPolicy.hpp>
#include <moreorg/policies/SelectionPolicy.hpp>

using namespace moreorg;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(policy)

BOOST_AUTO_TEST_CASE(get_instance)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI crex = vocabulary::OM::resolve("CREX");
    IRI payload = vocabulary::OM::resolve("Payload");

    ModelPool modelPool;
    modelPool[sherpa] = 1;
    modelPool[payload] = 1;
    modelPool[crex] = 1;
    Agent a(modelPool);

    OrganizationModelAsk ask(om, modelPool, true);

    Policy::Ptr policy = Policy::getInstance(
        vocabulary::OM::resolve("TransportProviderSelection"),
        ask);

    policies::Selection selection =
        dynamic_pointer_cast<policies::SelectionPolicy>(policy)->apply({a},
                                                                       ask);

    for(const Agent& a : selection)
    {
        BOOST_TEST_MESSAGE("Selected Agent: " << a.toString());
    }
}

BOOST_AUTO_TEST_CASE(distribution)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI crex = vocabulary::OM::resolve("CREX");
    IRI payload = vocabulary::OM::resolve("Payload");

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[payload] = 1;
        modelPool[crex] = 1;
        Agent a(modelPool);

        OrganizationModelAsk ask(om, modelPool, true);

        Policy::Ptr policy = Policy::getInstance(
            vocabulary::OM::resolve("DistributionPolicy_EnergyProvider"),
            ask);

        policies::Distribution distribution =
            dynamic_pointer_cast<policies::DistributionPolicy>(policy)->apply(
                a.getType(),
                ask);

        BOOST_REQUIRE_MESSAGE(distribution.shares[sherpa] > 0.8,
                              "Contribution of "
                                  << sherpa << " > 0.8, was "
                                  << distribution.shares[sherpa]);
        BOOST_REQUIRE_MESSAGE(distribution.shares[payload] == 0,
                              "Contribution of "
                                  << payload << " = 0, was "
                                  << distribution.shares[payload]);
        BOOST_REQUIRE_MESSAGE(distribution.shares[crex] > 0.15,
                              "Contribution of " << crex << " > 0.15, was "
                                                 << distribution.shares[crex]);
    }
    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        Agent a(modelPool);

        OrganizationModelAsk ask(om, modelPool, true);

        Policy::Ptr policy = Policy::getInstance(
            vocabulary::OM::resolve("DistributionPolicy_EnergyProvider"),
            ask);

        policies::Distribution distribution =
            dynamic_pointer_cast<policies::DistributionPolicy>(policy)->apply(
                a.getType(),
                ask);

        BOOST_REQUIRE_MESSAGE(distribution.shares[sherpa] = 1.0,
                              "Contribution of "
                                  << sherpa << " = 1.0, was "
                                  << distribution.shares[sherpa]);
    }
}

BOOST_AUTO_TEST_SUITE_END()
