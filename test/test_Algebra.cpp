#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <graph_analysis/BaseGraph.hpp>
#include <graph_analysis/GraphIO.hpp>
#include <moreorg/Algebra.hpp>
#include <moreorg/OrganizationModel.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/algebra/Connectivity.hpp>
#include <moreorg/vocabularies/OM.hpp>

using namespace moreorg;
using namespace moreorg::algebra;

BOOST_AUTO_TEST_SUITE(algebra)

BOOST_AUTO_TEST_CASE(max)
{
    ModelPool a;
    a["a"] = 1;
    a["b"] = 1;

    ModelPool b;
    b["a"] = 2;
    b["b"] = 1;
    b["c"] = 2;

    {
        ModelPool c = Algebra::max(a, b);
        BOOST_REQUIRE_MESSAGE(c["a"] == 2 && c["b"] == 1 && c["c"] == 2,
                              "ModelPool max: expected, a:2,b:1,c:2 got "
                                  << c.toString());
    }

    ModelPool c;
    c["a"] = 4;
    c["b"] = 0;
    c["c"] = 1;
    c["d"] = 1;

    ModelPool::List poolList;
    poolList.push_back(a);
    poolList.push_back(b);
    poolList.push_back(c);

    ModelPool maxPool = Algebra::max(poolList);
    BOOST_REQUIRE_MESSAGE(
        maxPool["a"] == 4 && maxPool["b"] == 1 && maxPool["c"] == 2 &&
            maxPool["d"] == 1,
        "ModelPool max of list: expected: a:4,b:1,c:2,d:1 got "
            << maxPool.toString());
}

BOOST_AUTO_TEST_CASE(min)
{
    ModelPool a;
    a["a"] = 1;
    a["b"] = 1;

    ModelPool b;
    b["a"] = 2;
    b["b"] = 0;
    b["c"] = 2;

    {
        ModelPool c = Algebra::min(a, b);
        BOOST_REQUIRE_MESSAGE(c["a"] == 1 && c["b"] == 0 && c["c"] == 2,
                              "ModelPool min: expected, a:1,b:0,c:2 got "
                                  << c.toString());
    }

    ModelPool c;
    c["a"] = 4;
    c["b"] = 1;
    c["c"] = 1;
    c["d"] = 1;

    ModelPool::List poolList;
    poolList.push_back(a);
    poolList.push_back(b);
    poolList.push_back(c);

    ModelPool minPool = Algebra::min(poolList);
    BOOST_REQUIRE_MESSAGE(
        minPool["a"] == 1 && minPool["b"] == 0 && minPool["c"] == 1 &&
            minPool["d"] == 1,
        "ModelPool min of list: expected: a:1,b:0,c:1,d:1 got "
            << minPool.toString());
}

BOOST_AUTO_TEST_CASE(resource_support)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace moreorg::vocabulary;

    OrganizationModel::Ptr om(
        new OrganizationModel(getRootDir() + "/test/data/om-schema-v0.7.owl"));
    OrganizationModelAsk ask(om);
    {

        IRI actor = OM::resolve("Actor");
        IRI sherpa = OM::resolve("Sherpa");
        IRI crex = OM::resolve("CREX");

        IRIList resources;
        resources.push_back(sherpa);
        resources.push_back(actor);
        resources.push_back(crex);

        base::VectorXd count = base::VectorXd::Zero(3);
        count(0) = 1;
        count(1) = 1;
        count(2) = 1;
        ResourceSupportVector supportVector(count, resources);

        ResourceSupportVector e_supportVector =
            supportVector.embedClassRelationship(ask);
        BOOST_TEST_MESSAGE("Test: " << e_supportVector.toString());
    }
    //{
    //    IRI system = OM::resolve("Sherpa");
    //    IRI service  = OM::resolve("StereoImageProvider");

    //    IRIList combination;
    //    combination.push_back(system);

    //    IRIList services;
    //    services.push_back(service);

    //    IRIList supportedServices = om.filterSupportedModels(combination,
    //    services); BOOST_REQUIRE_MESSAGE(supportedServices.size() == 1,
    //    "Sherpa support StereoImagerProvider");
    //}
}

BOOST_AUTO_TEST_CASE(resource_support_vector)
{
    {
        ResourceSupportVector nullSupport;
        BOOST_REQUIRE_MESSAGE(nullSupport.isNull(),
                              "ResourceSupportVector is null");
    }

    {
        base::VectorXd aSizes(3);
        aSizes(0) = 1;
        aSizes(1) = 1;
        aSizes(2) = 1;

        ResourceSupportVector a(aSizes);

        base::VectorXd bSizes(3);
        bSizes(0) = 1;
        bSizes(1) = 1;
        bSizes(2) = 0;
        ResourceSupportVector b(bSizes);

        ResourceSupportVector intersection =
            ResourceSupportVector::intersection(a, b);

        BOOST_REQUIRE_MESSAGE(intersection(0) == 1, "Dim 0 --> 1");
        BOOST_REQUIRE_MESSAGE(intersection(1) == 1, "Dim 1 --> 1");
        BOOST_REQUIRE_MESSAGE(intersection(2) == 0, "Dim 1 --> 0");

        BOOST_REQUIRE_MESSAGE(a.contains(b),
                              "ResourceSupportVector a contains b");
        BOOST_REQUIRE_MESSAGE(!b.contains(a),
                              "ResourceSupportVector b does not contain a");

        {
            ResourceSupportVector missing = a.missingSupportFrom(b);
            BOOST_REQUIRE_MESSAGE(
                missing(2) == 1,
                "Missing support in dimension 2: " << missing(2));
        }
    }

    {
        base::VectorXd aSizes(3);
        aSizes(0) = 1;
        aSizes(1) = 1;
        aSizes(2) = 1;

        ResourceSupportVector a(aSizes);

        base::VectorXd bSizes(3);
        bSizes(0) = 0;
        bSizes(1) = 0;
        bSizes(2) = 3;
        ResourceSupportVector b(bSizes);

        ResourceSupportVector intersection =
            ResourceSupportVector::intersection(a, b);

        BOOST_REQUIRE_MESSAGE(intersection(0) == 0, "Dim 0 --> 0");
        BOOST_REQUIRE_MESSAGE(intersection(1) == 0, "Dim 1 --> 0");
        BOOST_REQUIRE_MESSAGE(intersection(2) == 1, "Dim 1 --> 1");
    }
}

BOOST_AUTO_TEST_CASE(composition)
{
    ModelPool empty;

    ModelPool a;
    a["http://model#a"] = 1;
    a["http://model#b"] = 4;
    a["http://model#c"] = 3;

    {
        ModelPool::Set aSet;
        aSet.insert(a);
        ModelPool::Set rSet = Algebra::maxCompositions(a, empty);
        BOOST_REQUIRE_MESSAGE(rSet == aSet,
                              "Composition with model pool and empty set "
                              "results in first argument");
    }
    {
        ModelPool::Set aSet;
        aSet.insert(a);
        ModelPool::Set rSet = Algebra::maxCompositions(empty, a);
        BOOST_REQUIRE_MESSAGE(rSet == aSet,
                              "Composition with empty set and model pool "
                              "results in second argument");
    }

    ModelPool b;
    b["http://model#a"] = 2;
    b["http://model#b"] = 4;
    b["http://model#c"] = 1;

    {
        ModelPool::Set aSet;
        aSet.insert(a);

        ModelPool::Set bSet;
        bSet.insert(b);

        // With min operator
        ModelPool::Set cSet = Algebra::maxCompositions(a, b);
        BOOST_REQUIRE_MESSAGE(cSet.size() == 1,
                              "1 composition expected: was " << cSet.size());
        ModelPool c = *cSet.begin();
        BOOST_REQUIRE_MESSAGE(c["http://model#a"] == 2,
                              "Composition entry a->2 expected, was a->"
                                  << c["http://model#a"]);
        BOOST_REQUIRE_MESSAGE(c["http://model#b"] == 4,
                              "Composition entry b->4 expected, was b->"
                                  << c["http://model#b"]);
        BOOST_REQUIRE_MESSAGE(c["http://model#c"] == 3,
                              "Composition entry c->3 expected, was c->"
                                  << c["http://model#c"]);
    }
    {
        ModelPool::Set aSet;
        aSet.insert(a);

        ModelPool::Set bSet;
        bSet.insert(a);
        bSet.insert(b);
        ModelPool::Set cSet = Algebra::maxCompositions(aSet, bSet);
        BOOST_REQUIRE_MESSAGE(cSet.size() == 2,
                              "2 compositions expected: was 2 "
                                  << cSet.size() << ModelPool::toString(cSet));

        ModelPool e0 = a;
        BOOST_REQUIRE_MESSAGE(cSet.find(e0) != cSet.end(),
                              "Compositions contains 'a[+]a'");

        ModelPool e1;
        e1["http://model#a"] = 2;
        e1["http://model#b"] = 4;
        e1["http://model#c"] = 3;
        BOOST_REQUIRE_MESSAGE(cSet.find(e1) != cSet.end(),
                              "Compositions contains 'a[+]b'");
    }
    // Identical set
    {
        ModelPool::Set bSet;
        bSet.insert(a);
        bSet.insert(b);
        ModelPool::Set cSet = Algebra::maxCompositions(bSet, bSet);
        BOOST_REQUIRE_MESSAGE(cSet.size() == 3,
                              "3 compositions expected: was 3 "
                                  << cSet.size() << ModelPool::toString(cSet));

        ModelPool e0 = a;
        BOOST_REQUIRE_MESSAGE(cSet.find(e0) != cSet.end(),
                              "Compositions contains 'a[+]a'");

        ModelPool e1;
        e1["http://model#a"] = 2;
        e1["http://model#b"] = 4;
        e1["http://model#c"] = 3;
        BOOST_REQUIRE_MESSAGE(cSet.find(e1) != cSet.end(),
                              "Compositions contains 'a[+]b'");

        ModelPool e2 = b;
        BOOST_REQUIRE_MESSAGE(cSet.find(e1) != cSet.end(),
                              "Compositions contains 'b[+]b'");
    }
}

BOOST_AUTO_TEST_CASE(connectivity)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    OrganizationModelAsk ask(om);

    owlapi::model::IRI ifModel0 = vocabulary::OM::resolve("EmiActive");
    owlapi::model::IRI ifModel1 = vocabulary::OM::resolve("EmiPassive");

    BOOST_REQUIRE_MESSAGE(
        ask.ontology().isRelatedTo(ifModel0,
                                   vocabulary::OM::compatibleWith(),
                                   ifModel1),
        "Interfaces are compatible");
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 2;
        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 10;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 20;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 40;

        graph_analysis::BaseGraph::Ptr baseGraph;
        BOOST_REQUIRE_MESSAGE(
            Connectivity::isFeasible(modelPool, ask, baseGraph, 0),
            "ModelPool: " << modelPool.toString());
        graph_analysis::io::GraphIO::write(
            "/tmp/organization-model-connectivity-test-payload-10.dot",
            baseGraph);
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("CREX")] = 1;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("CREX")] = 2;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask, 30000),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 1;
        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 2;
        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 20;
        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 1;
        modelPool[vocabulary::OM::resolve("CREX")] = 1;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 1;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask, 30000),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 4;
        modelPool[vocabulary::OM::resolve("CREX")] = 3;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 10;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        graph_analysis::BaseGraph::Ptr baseGraph;
        BOOST_REQUIRE_MESSAGE(
            Connectivity::isFeasible(modelPool, ask, baseGraph, 0),
            "ModelPool: " << modelPool.toString());
        graph_analysis::io::GraphIO::write(
            "/tmp/organization-model-connectivity-test-sherpa10-cex10.dot",
            baseGraph);
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 4;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask, 30000),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 6;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        graph_analysis::BaseGraph::Ptr baseGraph;
        BOOST_REQUIRE_MESSAGE(
            !Connectivity::isFeasible(modelPool, ask, baseGraph, 30000),
            "ModelPool: " << modelPool.toString());
        if(baseGraph)
        {
            graph_analysis::io::GraphIO::write(
                "/tmp/organization-model-connectivity-test-sherpa6-cex10.dot",
                baseGraph);
        }
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 8;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask, 30000),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 9;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }

    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 5;
        modelPool[vocabulary::OM::resolve("CREX")] = 3;
        modelPool[vocabulary::OM::resolve("BaseCamp")] = 3;
        modelPool[vocabulary::OM::resolve("Payload")] = 10;

        // active: 2x5
        // passive: 3x3

        graph_analysis::BaseGraph::Ptr baseGraph;
        bool feasible =
            Connectivity::isFeasible(modelPool, ask, baseGraph, 60000);
        if(baseGraph)
        {
            graph_analysis::io::GraphIO::write(
                "/tmp/"
                "organization-model-connectivity-test-full-team_s5c3b5p25.dot",
                baseGraph);
        }
        BOOST_TEST_MESSAGE(
            "Evaluation done: " << Connectivity::getStatistics().toString());
        BOOST_REQUIRE_MESSAGE(feasible, "ModelPool: " << modelPool.toString());
    }
}

BOOST_AUTO_TEST_CASE(connectivity_multiple_interfaces)
{
    OrganizationModel::Ptr om(new OrganizationModel(
        getRootDir() + "/test/data/om-multiple-interfaces.owl"));
    OrganizationModelAsk ask(om);

    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 2;

        BOOST_REQUIRE_MESSAGE(
            !Connectivity::isFeasible(modelPool, ask),
            "RobotA 2 should not be a valid combination: ModelPool: "
                << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotB")] = 2;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotC")] = 2;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 1;
        modelPool[vocabulary::OM::resolve("RobotB")] = 1;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 1;
        modelPool[vocabulary::OM::resolve("RobotC")] = 1;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotB")] = 1;
        modelPool[vocabulary::OM::resolve("RobotC")] = 1;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 1;
        modelPool[vocabulary::OM::resolve("RobotB")] = 1;
        modelPool[vocabulary::OM::resolve("RobotC")] = 1;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask),
                              "ModelPool: " << modelPool.toString());
    }
}

BOOST_AUTO_TEST_CASE(subset_superset)
{
    ModelPool modelPoolA;
    ModelPool modelPoolB;

    BOOST_REQUIRE_MESSAGE(Algebra::isSubset(modelPoolA, modelPoolB),
                          "Empty models are subsets");
    BOOST_REQUIRE_MESSAGE(Algebra::isSuperset(modelPoolA, modelPoolB),
                          "Empty models are supersets");

    modelPoolA[vocabulary::OM::resolve("RobotA")] = 1;
    modelPoolB[vocabulary::OM::resolve("RobotA")] = 1;

    BOOST_REQUIRE_MESSAGE(Algebra::isSubset(modelPoolA, modelPoolB),
                          "Equal model pool are subsets");
    BOOST_REQUIRE_MESSAGE(Algebra::isSubset(modelPoolB, modelPoolA),
                          "Equal model pool are subsets");
    BOOST_REQUIRE_MESSAGE(Algebra::isSuperset(modelPoolA, modelPoolB),
                          "Equal model pool are supersets");
    BOOST_REQUIRE_MESSAGE(Algebra::isSuperset(modelPoolB, modelPoolA),
                          "Equal model pool are supersets");

    modelPoolA[vocabulary::OM::resolve("RobotB")] = 1;
    BOOST_REQUIRE_MESSAGE(!Algebra::isSubset(modelPoolA, modelPoolB),
                          "A is not subset of B");
    BOOST_REQUIRE_MESSAGE(Algebra::isSubset(modelPoolB, modelPoolA),
                          "B is subset of A");
    BOOST_REQUIRE_MESSAGE(Algebra::isSuperset(modelPoolA, modelPoolB),
                          "A is superset of B");
    BOOST_REQUIRE_MESSAGE(!Algebra::isSuperset(modelPoolB, modelPoolA),
                          "B is not a superset of A");
}

BOOST_AUTO_TEST_SUITE_END()
