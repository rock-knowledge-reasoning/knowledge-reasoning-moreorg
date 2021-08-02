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

        property = tell.objectProperty( has );

        tell.subClassOf(c,b);
        tell.subClassOf(b,a);
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

BOOST_FIXTURE_TEST_CASE(redundancy, RedundancyFixture )
{
    using namespace metrics;
    // query =2.has.A and =2.has.C
    std::vector<OWLCardinalityRestriction::Ptr> query, resourcePool;
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
    {
        OWLCardinalityRestriction::Ptr restriction =
            make_shared<OWLObjectExactCardinality>(property, 2, b);
        resourcePool.push_back(restriction);
    }
    {
        OWLCardinalityRestriction::Ptr restriction =
            make_shared<OWLObjectExactCardinality>(property, 2, c);
        resourcePool.push_back(restriction);
    }

    OrganizationModelAsk ask(om);
    Redundancy redundancy(ask, make_shared<ConstantPDF>(0.5), has);
    // Serial connection of two parallel a 0.5
    // [ [ 0.5 ] --- [ 0.5 ] ]  --- [ [ 0.5 ] --- [ 0.5 ] ]
    //
    // each serial system has a survivability of: 0.5*0.5 = 0.25
    // overall serial system: 0.25*0.25 = 0.0625
    double expected = 0.0625;
    double redundancyVal = redundancy.computeMetric(query, resourcePool);
    BOOST_REQUIRE_MESSAGE(redundancyVal == expected, "Redundancy expected: " << expected << " but got " << redundancyVal);
}

BOOST_FIXTURE_TEST_CASE(redundancy_same_parallel_in_series, RedundancyFixture)
{
    using namespace metrics;
    std::vector<OWLCardinalityRestriction::Ptr> query, resourcePool;
    {
        OWLCardinalityRestriction::Ptr restriction =
            make_shared<OWLObjectExactCardinality>(property, 2, a);
        query.push_back(restriction);
    }

    // Resources
    {
        {
            OWLCardinalityRestriction::Ptr restriction =
                make_shared<OWLObjectExactCardinality>(property, 2, a);
            resourcePool.push_back(restriction);
        }
        {
            OWLCardinalityRestriction::Ptr restriction =
                make_shared<OWLObjectExactCardinality>(property, 2, a);
            resourcePool.push_back(restriction);
        }

        OrganizationModelAsk ask(om);
        Redundancy redundancy(ask, make_shared<ConstantPDF>(0.5), has);
        // when the structure be considered then: (1 -pow( 1-pow(0.5,2),2))*pow(0.5,2);
        double expected = 0.25;
        double redundancyVal = redundancy.computeMetric(query, resourcePool);
        BOOST_REQUIRE_MESSAGE(redundancyVal == expected, "Redundancy expected: " << expected << " but got " << redundancyVal);
    }
    {
        {
            OWLCardinalityRestriction::Ptr restriction =
                make_shared<OWLObjectExactCardinality>(property, 4, a);
            resourcePool.push_back(restriction);
        }

        OrganizationModelAsk ask(om);
        Redundancy redundancy(ask, make_shared<ConstantPDF>(0.5), has);
        // --> (1 - (1-0.25)^2)
        double expected = 0.4375;
        double redundancyVal = redundancy.computeMetric(query, resourcePool);
        BOOST_REQUIRE_MESSAGE(redundancyVal == expected, "Redundancy expected: " << expected << " but got " << redundancyVal);
    }
}

BOOST_AUTO_TEST_CASE(redundancy_computation)
{
    OrganizationModel om(getRootDir() + "/test/data/om-project-transterra.owl");
    OrganizationModelAsk omAsk(OrganizationModel::Ptr(new OrganizationModel(om)) );
    metrics::Redundancy redundancy(omAsk);

    IRI sherpa = moreorg::vocabulary::OM::resolve("Sherpa");
    IRI payloadCamera = moreorg::vocabulary::OM::resolve("PayloadCamera");
    IRI payloadBattery = moreorg::vocabulary::OM::resolve("PayloadBattery");
    IRI transportProvider = moreorg::vocabulary::OM::resolve("TransportProvider");

    //{
    //    ModelPool availableModelPool;
    //    availableModelPool[sherpa] = 1;

    //    IRIList functionalities;
    //    functionalities.push_back(transportProvider);
    //    BOOST_TEST_MESSAGE("TEST: " << redundancy.computeSequential(functionalities, availableModelPool));
    //}

    IRI stereoCameraProvider = moreorg::vocabulary::OM::resolve("StereoImageProvider");
    {
        ModelPool availableModelPool;
        availableModelPool[sherpa] = 1;
        availableModelPool[payloadCamera] = 1;

        IRIList functionalities;
        functionalities.push_back(transportProvider);
        functionalities.push_back(stereoCameraProvider);
        BOOST_TEST_MESSAGE("TEST: " << redundancy.computeSequential(functionalities, availableModelPool));
    }

    //{
    //    ModelPool availableModelPool;
    //    availableModelPool[sherpa] = 1;

    //    ModelPool requiredModelPool;
    //    requiredModelPool[sherpa] = 1;
    //    requiredModelPool[transportProvider] = 1;

    //    // todo: redundancy when nothing is required
    //    BOOST_REQUIRE_THROW( redundancy.computeExclusiveUse(requiredModelPool, availableModelPool), std::runtime_error );

    //    availableModelPool[sherpa] = 2;
    //    double r =  redundancy.computeExclusiveUse(requiredModelPool, availableModelPool);

    //    BOOST_TEST_MESSAGE("Test show required: " << OWLCardinalityRestriction::toString( omAsk.getCardinalityRestrictions(requiredModelPool, moreorg::vocabulary::OM::has(), OWLCardinalityRestriction::SUM_OP) ) );
    //    BOOST_TEST_MESSAGE("Test show available: " << OWLCardinalityRestriction::toString( omAsk.getCardinalityRestrictions(availableModelPool, moreorg::vocabulary::OM::has(), OWLCardinalityRestriction::SUM_OP) ) );
    //    BOOST_REQUIRE_MESSAGE(true, "Redundancy exclusive use is:" << r);
    //}

    //{
    //    ModelPool availableModelPool;
    //    availableModelPool[sherpa] = 1;

    //    ModelPool requiredModelPool;
    //    requiredModelPool[sherpa] = 1;
    //    requiredModelPool[transportProvider] = 1;

    //    // todo: redundancy when nothing is required
    //    double r = redundancy.computeSharedUse(requiredModelPool, availableModelPool);
    //    BOOST_REQUIRE_MESSAGE(true, "Redundancy shared use is:" << r);
    //}
}

BOOST_AUTO_TEST_CASE(function_redundancy)
{
    std::string filename = getRootDir() + "/test/data/om-project-transterra.owl";

    ModelPool modelPool;
    modelPool[moreorg::vocabulary::OM::resolve("Sherpa")] = 5;
    modelPool[moreorg::vocabulary::OM::resolve("CREX")] = 3;
    modelPool[moreorg::vocabulary::OM::resolve("BaseCamp")] = 3;
    modelPool[moreorg::vocabulary::OM::resolve("Payload")] = 10;

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(filename);
    OrganizationModelAsk ask(om, modelPool, true);

    {
        IRISet functionSet;
        functionSet.insert(moreorg::vocabulary::OM::resolve("Mapping"));
        Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
        double value = metrics->computeSharedUse(functionSet, modelPool);
        BOOST_REQUIRE_MESSAGE(value != 0, "Mapping has redundancy: " << value);
    }

    {
        IRISet functionSet;
        functionSet.insert(moreorg::vocabulary::OM::resolve("TransportProvider"));
        Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
        double value = metrics->computeSharedUse(functionSet, modelPool);

        BOOST_REQUIRE_MESSAGE(value != 0, "TransportProvider has redundancy: " << value);
    }
}

BOOST_AUTO_TEST_CASE(probability_density_function)
{
  using namespace metrics;
  std::string filename = getRootDir() + "/test/data/om-project-transterra.owl";

  ModelPool modelPool;
  modelPool[moreorg::vocabulary::OM::resolve("Sherpa")] = 1;

  OrganizationModel::Ptr om = make_shared<OrganizationModel>(filename);
  OrganizationModelAsk ask(om, modelPool, true);

  ProbabilityDensityFunction::Ptr pdf_ptr = ProbabilityDensityFunction::getInstance(ask, IRI("http://www.rock-robotics.org/2015/12/robots/Sherpa#Sherpa_Camera_Front"));
  BOOST_REQUIRE_MESSAGE(pdf_ptr, "Probability Density Function has been created");

  BOOST_TEST_MESSAGE("Probability Density Value at t = 0: " << pdf_ptr->getValue());
}

BOOST_FIXTURE_TEST_CASE(probability_of_failure, RedundancyFixture)
{
    using namespace metrics;

    std::string filename = getRootDir() + "/test/data/om-project-transterra.owl";

    ModelPool modelPool;
    modelPool[moreorg::vocabulary::OM::resolve("Sherpa")] = 1;

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(filename);
    OrganizationModelAsk ask(om, modelPool, true);

    ProbabilityDensityFunction::Ptr pdf_ptr = make_shared<WeibullPDF>(1, 1);
    
    OWLObjectCardinalityRestriction::Ptr restriction = make_shared<OWLObjectExactCardinality>(property, 2, a);   

    
    ProbabilityOfFailure PoF(restriction, pdf_ptr, 1.);
    BOOST_TEST_MESSAGE("Probability of Failure Conditional Value at t0 = 0, t1 = 0: " << PoF.getProbabilityOfFailureConditional(0., 0.));

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
