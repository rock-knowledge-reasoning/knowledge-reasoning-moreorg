#include <boost/test/unit_test.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/InferenceRule.hpp>
#include "test_utils.hpp"
#include <moreorg/vocabularies/OM.hpp>
#include <moreorg/facades/Robot.hpp>


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

    InferenceRule::Ptr r = InferenceRule::loadCompositeAgentRule(vocabulary::OM::resolve("transportCapacity"), ask);

    BOOST_TEST_MESSAGE(r->toString());

    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);

    double value = r->apply(robot);
    BOOST_REQUIRE_MESSAGE(value == 9, "Transport capacity of composite agent: "
            << value << " expected 9");
}

BOOST_AUTO_TEST_CASE(infer_energyCapacity_atomic)
{

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI payload = vocabulary::OM::resolve("Payload");


    ModelPool modelPool;
    modelPool[sherpa] = 1;
    OrganizationModelAsk ask(om, modelPool, true);

    IRI property = vocabulary::OM::resolve("InferenceRule_energyCapacity");
    OWLAnnotationValue::Ptr ruleTxt =
        ask.ontology().getAnnotationValue(property, vocabulary::OM::resolve("inferFrom"));

    OWLLiteral::Ptr literal = ruleTxt->asLiteral();
    BOOST_REQUIRE_MESSAGE(literal, "Literal annotation for " <<
            property.toString() << " " << literal);
    BOOST_TEST_MESSAGE("VALUE: " << literal->getValue());

    InferenceRule::Ptr r =
        InferenceRule::loadAtomicAgentRule(vocabulary::OM::resolve("energyCapacity"), ask);

    BOOST_TEST_MESSAGE(r->toString());

    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
    double value = r->apply(robot);
    BOOST_TEST_MESSAGE("EVALUTED " << value);
}

BOOST_AUTO_TEST_CASE(infer_energyCapacity)
{

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    IRI sherpa = vocabulary::OM::resolve("Sherpa");
    IRI payload = vocabulary::OM::resolve("Payload");

    //{
    //    ModelPool modelPool;
    //    modelPool[sherpa] = 1;
    //    OrganizationModelAsk ask(om, modelPool, true);

    //    IRI energyCapacity = vocabulary::OM::resolve("energyCapacity");
    //    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
    //    double value = robot.getDataPropertyValue(energyCapacity);
    //    BOOST_REQUIRE_MESSAGE(value == 480, "Sherpa has power capacity of 480, but was " << value);
    //}

    //{
    //    ModelPool modelPool;
    //    modelPool[payload] = 1;
    //    OrganizationModelAsk ask(om, modelPool, true);

    //    IRI energyCapacity = vocabulary::OM::resolve("energyCapacity");
    //    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
    //    double value = robot.getDataPropertyValue(energyCapacity);
    //    BOOST_REQUIRE_MESSAGE(value == 0, "Payload has no energy capacity, but was " << value);
    //}

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[payload] = 1;
        OrganizationModelAsk ask(om, modelPool, true);

        IRI energyCapacity = vocabulary::OM::resolve("energyCapacity");
        facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
        double value = robot.getDataPropertyValue(energyCapacity);
        BOOST_TEST_MESSAGE("EVALUATED " << value);
    }
}


BOOST_AUTO_TEST_SUITE_END()
