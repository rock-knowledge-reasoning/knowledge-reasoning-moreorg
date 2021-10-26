#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <moreorg/OrganizationModel.hpp>
#include <moreorg/metrics/Redundancy.hpp>
#include <moreorg/metrics/ProbabilityOfFailure.hpp>
#include <moreorg/vocabularies/OM.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/model/OWLObjectExactCardinality.hpp>
#include <moreorg/vocabularies/OMBase.hpp>
#include <moreorg/Agent.hpp>
#include <moreorg/reasoning/ModelBound.hpp>

using namespace moreorg;
using namespace owlapi;
using namespace owlapi::model;

struct RedundancyFixture
{
    RedundancyFixture()
        : has("http://property/has")
    {
        om = make_shared<OrganizationModel>(getOMSchema());

        OWLOntologyTell tell(om->ontology());

        a = tell.klass("http://klass/base");
        b = tell.klass("http://klass/base-derived");
        c = tell.klass("http://klass/base-derived-derived");

        property = tell.objectProperty(has);

        tell.subClassOf(c, b);
        tell.subClassOf(b, a);
        om->ontology()->refresh();
    }

    IRI has;

    OWLClass::Ptr a;
    OWLClass::Ptr b;
    OWLClass::Ptr c;

    OWLObjectProperty::Ptr property;
    OrganizationModel::Ptr om;
};

BOOST_AUTO_TEST_SUITE(model_metrics)

BOOST_FIXTURE_TEST_CASE(redundancy, RedundancyFixture)
{
    using namespace metrics;
    // query =2.has.A and =2.has.C
    std::vector<OWLCardinalityRestriction::Ptr> query;
    {
        OWLCardinalityRestriction::Ptr restriction =
            make_shared<OWLObjectExactCardinality>(property, 2, a);
        query.push_back(restriction);
    }
    {
        OWLCardinalityRestriction::Ptr restriction =
            make_shared<OWLObjectExactCardinality>(property, 2, c);
        query.push_back(restriction);
    }

    // available =2.has.B and 2.has.C

    Agent agent;
    AtomicAgent b0(0, b->getIRI());
    AtomicAgent b1(1, b->getIRI());
    AtomicAgent c0(0, c->getIRI());
    AtomicAgent c1(1, c->getIRI());

    agent.add(b0);
    agent.add(b1);
    agent.add(c0);
    agent.add(c1);

    OrganizationModelAsk ask(om);
    ResourceInstance::List available = ask.getRelated(agent);
    Redundancy redundancy(ask, make_shared<ConstantPDF>(0.5), has);
    // Serial connection of two parallel a 0.5
    // [ [ 0.5 ] --- [ 0.5 ] ]  --- [ [ 0.5 ] --- [ 0.5 ] ]
    //
    // each serial system has a survivability of: 0.5*0.5 = 0.25
    // overall serial system: 0.25*0.25 = 0.0625
    double expected = 0.0625;
    double redundancyVal = redundancy.computeMetric(query, available);
    BOOST_REQUIRE_MESSAGE(redundancyVal == expected, "Redundancy expected: " << expected << " but got " << redundancyVal);
}

BOOST_FIXTURE_TEST_CASE(redundancy_same_parallel_in_series, RedundancyFixture)
{
    using namespace metrics;
    std::vector<OWLCardinalityRestriction::Ptr> query;
    {
        OWLCardinalityRestriction::Ptr restriction =
            make_shared<OWLObjectExactCardinality>(property, 2, a);
        query.push_back(restriction);
    }

    // Resources
    {
        Agent agent;
        AtomicAgent a0(0, a->getIRI());
        AtomicAgent a1(1, a->getIRI());
        AtomicAgent a2(2, a->getIRI());
        AtomicAgent a3(3, a->getIRI());

        agent.add(a0);
        agent.add(a1);
        agent.add(a2);
        agent.add(a3);

        OrganizationModelAsk ask(om);
        ResourceInstance::List available = ask.getRelated(agent);
        Redundancy redundancy(ask, make_shared<ConstantPDF>(0.5), has);
        // when the structure be considered then: (1 -pow( 1-pow(0.5,2),2))*pow(0.5,2);
        double expected = 0.25;
        double redundancyVal = redundancy.computeMetric(query, available);
        BOOST_REQUIRE_MESSAGE(redundancyVal == expected, "Redundancy expected: " << expected << " but got " << redundancyVal);
    }
}

BOOST_AUTO_TEST_CASE(redundancy_computation)
{
    OrganizationModel om(getRootDir() + "/test/data/om-project-transterra.owl");
    OrganizationModelAsk omAsk(OrganizationModel::Ptr(new OrganizationModel(om)));
    metrics::Redundancy redundancy(omAsk);

    IRI sherpa = moreorg::vocabulary::OM::resolve("Sherpa");
    IRI payloadCamera = moreorg::vocabulary::OM::resolve("PayloadCamera");
    IRI payloadBattery = moreorg::vocabulary::OM::resolve("PayloadBattery");
    IRI transportProvider = moreorg::vocabulary::OM::resolve("TransportProvider");

    double r_1, r_2, r_3;
    {
        ModelPool availableModelPool;
        availableModelPool[sherpa] = 1;

        IRIList functionalities;
        functionalities.push_back(transportProvider);
        r_1 = redundancy.computeSequential(functionalities, availableModelPool);
        BOOST_REQUIRE_MESSAGE(r_1 > 0.7, "Redundancy should be > 0.7, was: " << r_1);
    }

    {
        ModelPool availableModelPool;
        availableModelPool[sherpa] = 1;
        availableModelPool[payloadCamera] = 1;

        IRIList functionalities;
        functionalities.push_back(transportProvider);
        r_2 = redundancy.computeSequential(functionalities, availableModelPool);
        BOOST_REQUIRE_MESSAGE(r_2 > r_1, "Redundancy should be > " << r_1 << " was:" << r_2);
    }

    IRI stereoCameraProvider = moreorg::vocabulary::OM::resolve("StereoImageProvider");
    {
        Agent agent;
        AtomicAgent sherpa0(0, sherpa);
        AtomicAgent payloadCamera0(0, payloadCamera);
        agent.add(sherpa0);
        agent.add(payloadCamera0);

        ResourceInstance::List available = omAsk.getRelated(agent);

        IRIList functionalities;
        functionalities.push_back(transportProvider);
        functionalities.push_back(stereoCameraProvider);
        r_3 = redundancy.computeSequential(functionalities, available);
        BOOST_REQUIRE_MESSAGE(r_3 > 0.7, "Redundancy should be > 0.7 was: " << r_3);
    }

    {
        ModelPool availableModelPool;
        availableModelPool[sherpa] = 1;

        ModelPool requiredModelPool;
        requiredModelPool[sherpa] = 1;
        requiredModelPool[transportProvider] = 1;

        // todo: redundancy when nothing is required
        BOOST_REQUIRE_NO_THROW( redundancy.computeExclusiveUse(requiredModelPool, availableModelPool));

        availableModelPool[sherpa] = 2;
        double r =  redundancy.computeExclusiveUse(requiredModelPool, availableModelPool);

        BOOST_TEST_MESSAGE("Test show required: " << OWLCardinalityRestriction::toString( omAsk.getCardinalityRestrictions(requiredModelPool, moreorg::vocabulary::OM::has(), OWLCardinalityRestriction::SUM_OP) ) );
        BOOST_TEST_MESSAGE("Test show available: " << OWLCardinalityRestriction::toString( omAsk.getCardinalityRestrictions(availableModelPool, moreorg::vocabulary::OM::has(), OWLCardinalityRestriction::SUM_OP) ) );
        BOOST_REQUIRE_MESSAGE(r > 0.4, "Redundancy exclusive use is:" << r);
    }

    {
        ModelPool availableModelPool;
        availableModelPool[sherpa] = 1;

        ModelPool requiredModelPool;
        requiredModelPool[sherpa] = 1;

        owlapi::model::IRI image_provider = moreorg::vocabulary::OM::resolve("ImageProvider");

    //    // todo: redundancy when nothing is required
    //    double r = redundancy.computeSharedUse(requiredModelPool, availableModelPool);
    //    BOOST_REQUIRE_MESSAGE(true, "Redundancy shared use is:" << r);
    }
}

BOOST_AUTO_TEST_CASE(function_redundancy)
{
    std::string filename = getRootDir() + "/test/data/om-project-transterra.owl";
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(filename);

    {
        ModelPool modelPool;
        modelPool[moreorg::vocabulary::OM::resolve("Sherpa")] = 5;
        modelPool[moreorg::vocabulary::OM::resolve("CREX")] = 3;
        modelPool[moreorg::vocabulary::OM::resolve("BaseCamp")] = 3;
        modelPool[moreorg::vocabulary::OM::resolve("Payload")] = 10;

        Agent agent;
        AtomicAgent sherpa0(0, moreorg::vocabulary::OM::resolve("Sherpa"));
        AtomicAgent sherpa1(1, moreorg::vocabulary::OM::resolve("Sherpa"));
        AtomicAgent sherpa2(2, moreorg::vocabulary::OM::resolve("Sherpa"));
        AtomicAgent sherpa3(3, moreorg::vocabulary::OM::resolve("Sherpa"));
        AtomicAgent sherpa4(4, moreorg::vocabulary::OM::resolve("Sherpa"));
        agent.add(sherpa0);
        agent.add(sherpa1);
        agent.add(sherpa2);
        agent.add(sherpa3);
        AtomicAgent crex0(0, moreorg::vocabulary::OM::resolve("CREX"));
        AtomicAgent crex1(1, moreorg::vocabulary::OM::resolve("CREX"));
        AtomicAgent crex2(2, moreorg::vocabulary::OM::resolve("CREX"));
        agent.add(crex0);
        agent.add(crex1);
        agent.add(crex2);
        AtomicAgent baseCamp0(0, moreorg::vocabulary::OM::resolve("BaseCamp"));
        AtomicAgent baseCamp1(1, moreorg::vocabulary::OM::resolve("BaseCamp"));
        AtomicAgent baseCamp2(2, moreorg::vocabulary::OM::resolve("BaseCamp"));
        agent.add(baseCamp0);
        agent.add(baseCamp1);
        agent.add(baseCamp2);
        AtomicAgent payload0(0, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload1(1, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload2(2, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload3(3, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload4(4, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload5(5, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload6(6, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload7(7, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload8(8, moreorg::vocabulary::OM::resolve("Payload"));
        AtomicAgent payload9(9, moreorg::vocabulary::OM::resolve("Payload"));
        agent.add(payload0);
        agent.add(payload1);
        agent.add(payload2);
        agent.add(payload3);
        agent.add(payload4);
        agent.add(payload5);
        agent.add(payload6);
        agent.add(payload7);
        agent.add(payload8);
        agent.add(payload9);
        OrganizationModel::Ptr om = make_shared<OrganizationModel>(filename);
        OrganizationModelAsk ask(om, modelPool, true);
    }

    ResourceInstance::List available = ask.getRelated(agent);

    {
        IRISet functionSet;
        functionSet.insert(moreorg::vocabulary::OM::resolve("Mapping"));
        Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
        double value = metrics->computeSharedUse(functionSet, available);
        BOOST_REQUIRE_MESSAGE(value != 0, "Mapping has redundancy: " << value);
    }

    {
        IRISet functionSet;
        functionSet.insert(moreorg::vocabulary::OM::resolve("TransportProvider"));
        Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
        double value = metrics->computeSharedUse(functionSet, available);
        ModelPool minRequired;
        minRequired[moreorg::vocabulary::OM::resolve("ImageProvider")] = 1;

        ModelPool minAvailable;
        minAvailable[moreorg::vocabulary::OM::resolve("SherpaTT")] = 1;

        OrganizationModelAsk ask(om, minAvailable, true);

        {
            Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
            double value = metrics->computeSharedUse(minRequired, minAvailable);
            BOOST_REQUIRE_MESSAGE(value > 0.0, "Shared use should be > 0, was "
                    << value);
        }
    }

    {
        ModelPool minRequired;
        minRequired[moreorg::vocabulary::OM::resolve("SherpaTT")] = 1;

        ModelPool minAvailable;
        minAvailable[moreorg::vocabulary::OM::resolve("SherpaTT")] = 1;

        OrganizationModelAsk ask(om, minAvailable, true);

        {
            Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
            double value = metrics->computeSharedUse(minRequired, minAvailable);
            BOOST_REQUIRE_MESSAGE(value > 0.0, "Shared use should be > 0, was "
                    << value);
        }
    }
}

BOOST_AUTO_TEST_CASE(probability_density_function)
{
    using namespace metrics;
    std::string filename = getRootDir() + "/test/data/om-project-transterra.owl";

    ModelPool modelPool;
    modelPool[moreorg::vocabulary::OM::resolve("Sherpa")] = 1;

    Agent agent;
    AtomicAgent sherpa0(0, moreorg::vocabulary::OM::resolve("Sherpa"));
    agent.add(sherpa0);

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(filename);
    OrganizationModelAsk ask(om, modelPool, true);

    ResourceInstance::List available = ask.getRelated(agent);

    ProbabilityDensityFunction::Ptr pdf_ptr = ProbabilityDensityFunction::getInstance(ask, IRI("http://www.rock-robotics.org/2015/12/robots/Sherpa#Sherpa_Camera_Front"));
    BOOST_REQUIRE_MESSAGE(pdf_ptr, "Probability Density Function has been created");

    BOOST_TEST_MESSAGE("Probability Density Value at t = 0: " << pdf_ptr->getValue());

    ProbabilityDensityFunction::Ptr weibull_pdf_ptr = make_shared<WeibullPDF>(36000., 1.); // 36000s = 10h
    BOOST_TEST_MESSAGE("Weibull Probability Density Value at t = 0: " << weibull_pdf_ptr->getValue());
    BOOST_TEST_MESSAGE("Weibull Conditional Probability Value at t0 = 0, t1 = 0: " << weibull_pdf_ptr->getConditional());
}

BOOST_FIXTURE_TEST_CASE(probability_of_failure, RedundancyFixture)
{
    using namespace metrics;

    std::string filename = getRootDir() + "/test/data/om-project-transterra.owl";

    reasoning::ModelBound requirement(moreorg::vocabulary::OM::resolve("Sherpa"), 1, 1);

    ModelPool modelPool;
    modelPool[moreorg::vocabulary::OM::resolve("Sherpa")] = 1;

    Agent agent;
    AtomicAgent sherpa0(0, moreorg::vocabulary::OM::resolve("Sherpa"));
    agent.add(sherpa0);

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(filename);
    OrganizationModelAsk ask(om, modelPool, true);

    ResourceInstance::List available = ask.getRelated(agent);

    ProbabilityDensityFunction::Ptr pdf_ptr = make_shared<WeibullPDF>(36000., 1.);

    ProbabilityOfFailure PoF(requirement, available, pdf_ptr);
    BOOST_TEST_MESSAGE("Probability of Failure Conditional Value at t0 = 0, t1 = 0: " << PoF.getProbabilityOfFailureConditionalWithRedundancy(0., 0.));
}

BOOST_AUTO_TEST_CASE(metric_map_computation)
{
    //    OrganizationModel om( getOMSchema() );
    //    om.ontology()->refresh();
    //    metrics::Redundancy redundancy(om);
    //
    //    MetricMap survivability = redundancy.getMetricMap();
    //
    //    //BOOST_TEST_MESSAGE("Survivability: \n" << metrics::Redundancy::toString(survivability));
    //
    //    {
    //        IRI locationImageProvider("http://www.rock-robotics.org/2014/01/om-schema#ImageProvider");
    //        IRI sherpa("http://www.rock-robotics.org/2014/01/om-schema#Sherpa");
    //
    //        ModelPool modelPool;
    //        for(int i = 1; i <= 3; ++i)
    //        {
    //            modelPool[sherpa] = i;
    //            double pSurvivability = redundancy.compute(locationImageProvider, modelPool);
    //            BOOST_TEST_MESSAGE("Survivability for #" << i << " " << sherpa << ": \n" << pSurvivability);
    //        }
    //    }
    //    {
    //        IRI locationImageProvider("http://www.rock-robotics.org/2014/01/om-schema#LocationImageProvider");
    //        IRI crex("http://www.rock-robotics.org/2014/01/om-schema#CREX");
    //
    //        ModelPool modelPool;
    //        for(int i = 1; i <= 3; ++i)
    //        {
    //            modelPool[crex] = i;
    //
    //            //    required:
    //            //            ModelBound::List
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Camera' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Localization' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Locomotion' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Mapping' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#PowerSource' (1, 1000000)
    //            //
    //            //    available:
    //            //            ModelBound::List
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Camera' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#EmiPassive' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#LaserScanner' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Localization' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Locomotion' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Mapping' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#PowerSource' (0, 3)
    //
    //            double redundancyLevel = i;
    //            double parallel = 1 - pow((1-0.5),redundancyLevel);
    //            double expected = pow(parallel,5);
    //            double pSurvivability = redundancy.compute(locationImageProvider, modelPool);
    //            BOOST_REQUIRE_MESSAGE(pSurvivability == expected, "Survivability for #" << i << " " << crex << ": \n" << pSurvivability << " (expected: " << expected << ")");
    //        }
    //    }
    //    {
    //        IRI locationImageProvider("http://www.rock-robotics.org/2014/01/om-schema#LocationImageProvider");
    //        IRI stereoCamera("http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider");
    //
    //        IRISet services;
    //        services.insert(locationImageProvider);
    //        services.insert(stereoCamera);
    //
    //        IRI robot("http://www.rock-robotics.org/2014/01/om-schema#Sherpa");
    //        ModelPool modelPool;
    //        for(int i = 1; i <= 3; ++i)
    //        {
    //            modelPool[robot] = i;
    //
    //            //    required:
    //            //            ModelBound::List
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Camera' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Localization' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Locomotion' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Mapping' (1, 1000000)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#PowerSource' (1, 1000000)
    //            //
    //            //    available:
    //            //            ModelBound::List
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Camera' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#EmiPassive' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#LaserScanner' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Localization' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Locomotion' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#Mapping' (0, 3)
    //            //            ModelBound: 'http://www.rock-robotics.org/2014/01/om-schema#PowerSource' (0, 3)
    //
    //            double redundancyLevel = i;
    //            double parallel = 1 - pow((1-0.5),redundancyLevel);
    //            double expected = pow(parallel,5);
    //
    //            double pSurvivability = redundancy.computeSharedUse(services, modelPool);
    //            BOOST_TEST_MESSAGE("Survivability for #" << i << " " << robot << ": \n" << pSurvivability);
    //            //BOOST_REQUIRE_MESSAGE(pSurvivability == expected, "Survivability for #" << i << " " << crex << ": \n" << pSurvivability << " (expected: " << expected << ")");
    //        }
    //    }
}

BOOST_AUTO_TEST_SUITE_END()
