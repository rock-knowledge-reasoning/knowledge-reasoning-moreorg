#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <moreorg/InferenceRule.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/facades/Robot.hpp>
#include <moreorg/vocabularies/OM.hpp>

using namespace moreorg;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(inference_rule)

BOOST_AUTO_TEST_CASE(infer_transportCapacity)
{

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI payload = vocabulary::OM::resolve("Payload");

    ModelPool modelPool;
    modelPool[sherpa] = 1;
    modelPool[payload] = 1;
    OrganizationModelAsk ask(om, modelPool, true);

    InferenceRule::Ptr r = InferenceRule::loadPropertyCompositeAgentRule(
        vocabulary::OM::resolve("transportCapacity"),
        ask);

    BOOST_TEST_MESSAGE(r->toString());

    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);

    double value = r->apply(robot);
    BOOST_REQUIRE_MESSAGE(
        value == 9,
        "Transport capacity of composite agent: " << value << " expected 9");
}

BOOST_AUTO_TEST_CASE(infer_energyCapacity_atomic)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI payload = vocabulary::OM::resolve("Payload");

    ModelPool modelPool;
    modelPool[sherpa] = 1;
    OrganizationModelAsk ask(om, modelPool, true);

    IRI property = vocabulary::OM::resolve("AtomicAgentRule_energyCapacity");
    OWLAnnotationValue::Ptr ruleTxt =
        ask.ontology().getAnnotationValue(property,
                                          vocabulary::OM::resolve("inferFrom"));

    OWLLiteral::Ptr literal = ruleTxt->asLiteral();
    BOOST_REQUIRE_MESSAGE(literal,
                          "Literal annotation for " << property.toString()
                                                    << " " << literal);
    BOOST_TEST_MESSAGE(
        "AtomicAgentRule_energyCapacity: " << literal->getValue());

    InferenceRule::Ptr r = InferenceRule::loadPropertyAtomicAgentRule(
        vocabulary::OM::resolve("energyCapacity"),
        ask);

    BOOST_TEST_MESSAGE(r->toString());

    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
    double value = r->apply(robot);
    BOOST_REQUIRE_MESSAGE(value == 480,
                          "Sherpa has energyCapacity of " << value
                                                          << " expected 480");
}

BOOST_AUTO_TEST_CASE(infer_energyCapacity_composite)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI payload = vocabulary::OM::resolve("Payload");
    IRI crex = vocabulary::OM::resolve("CREX");

    ModelPool modelPool;
    modelPool[sherpa] = 1;
    modelPool[payload] = 1;
    modelPool[crex] = 1;
    OrganizationModelAsk ask(om, modelPool, true);

    IRI property = vocabulary::OM::resolve("CompositeAgentRule_energyCapacity");
    InferenceRule::Ptr r = InferenceRule::loadCompositeAgentRule(property, ask);

    BOOST_TEST_MESSAGE(r->toString());

    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
    double value = r->apply(robot);
    BOOST_REQUIRE_MESSAGE(value > 480,
                          "Sherpa+Payload+CREX have energyCapacity of "
                              << value << " expected >480");
}

BOOST_AUTO_TEST_CASE(infer_energyCapacity)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI payload = vocabulary::OM::resolve("Payload");

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        OrganizationModelAsk ask(om, modelPool, true);

        IRI energyCapacity = vocabulary::OM::resolve("energyCapacity");
        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        double value = robot.getDataPropertyValue(energyCapacity);
        BOOST_REQUIRE_MESSAGE(value == 480,
                              "Sherpa has power capacity of 480, but was "
                                  << value);
    }

    {
        ModelPool modelPool;
        modelPool[payload] = 1;
        OrganizationModelAsk ask(om, modelPool, true);

        IRI energyCapacity = vocabulary::OM::resolve("energyCapacity");
        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        double value = robot.getDataPropertyValue(energyCapacity);
        BOOST_REQUIRE_MESSAGE(value == 0,
                              "Payload has no energy capacity, but was "
                                  << value);
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[payload] = 1;
        OrganizationModelAsk ask(om, modelPool, true);

        IRI energyCapacity = vocabulary::OM::resolve("energyCapacity");
        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        double value = robot.getDataPropertyValue(energyCapacity);
        BOOST_REQUIRE_MESSAGE(value == 480,
                              "Sherpa+Payload have energy capacity"
                              " of "
                                  << value << ", expected 480");
    }
}

BOOST_AUTO_TEST_CASE(infer_nominalVelocity)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI payload = vocabulary::OM::resolve("Payload");
    IRI nominalVelocity = vocabulary::OM::resolve("nominalVelocity");

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        OrganizationModelAsk ask(om, modelPool, true);
        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        double value = robot.getDataPropertyValue(nominalVelocity);
        BOOST_REQUIRE_MESSAGE(value == 0.5,
                              "Sherpa has nominal velocity of 0.5 m/s, but was "
                                  << value);
    }

    {
        ModelPool modelPool;
        modelPool[payload] = 1;
        OrganizationModelAsk ask(om, modelPool, true);

        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        double value = robot.getDataPropertyValue(nominalVelocity);
        BOOST_REQUIRE_MESSAGE(value == 0,
                              "Payload has no nominal velocity, but was "
                                  << value);
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[payload] = 1;
        OrganizationModelAsk ask(om, modelPool, true);

        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        double value = robot.getDataPropertyValue(nominalVelocity);
        BOOST_REQUIRE_MESSAGE(
            value == 0.5,
            "Sherpa+Payload have nominalVelocity of 0.5 m/s but was " << value);
    }
}

BOOST_AUTO_TEST_SUITE_END()
