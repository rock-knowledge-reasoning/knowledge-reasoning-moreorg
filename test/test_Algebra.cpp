#include <boost/test/unit_test.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/Algebra.hpp>
#include "test_utils.hpp"
#include <organization_model/vocabularies/OM.hpp>
#include <organization_model/algebra/Connectivity.hpp>

using namespace organization_model;
using namespace organization_model::algebra;

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
        ModelPool c = Algebra::max(a,b);
        BOOST_REQUIRE_MESSAGE( c["a"] == 2 && c["b"] == 1 && c["c"] == 2, "ModelPool max: expected, a:2,b:1,c:2 got " << c.toString());
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
    BOOST_REQUIRE_MESSAGE(maxPool["a"] == 4 && maxPool["b"] == 1 && maxPool["c"] == 2 && maxPool["d"] == 1,
            "ModelPool max of list: expected: a:4,b:1,c:2,d:1 got " << maxPool.toString());
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
        ModelPool c = Algebra::min(a,b);
        BOOST_REQUIRE_MESSAGE( c["a"] == 1 && c["b"] == 0 && c["c"] == 2, "ModelPool min: expected, a:1,b:0,c:2 got " << c.toString());
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
    BOOST_REQUIRE_MESSAGE(minPool["a"] == 1 && minPool["b"] == 0 && minPool["c"] == 1 && minPool["d"] == 1,
            "ModelPool min of list: expected: a:1,b:0,c:1,d:1 got " << minPool.toString());
}

BOOST_AUTO_TEST_CASE(resource_support)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;
    using namespace organization_model::vocabulary;

    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-schema-v0.7.owl"));
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

        ResourceSupportVector e_supportVector = supportVector.embedClassRelationship(ask);
        BOOST_TEST_MESSAGE("Test: " << e_supportVector.toString());
    }
    //{
    //    IRI system = OM::resolve("Sherpa");
    //    IRI service  = OM::resolve("StereoImageProvider");

    //    IRIList combination;
    //    combination.push_back(system);

    //    IRIList services;
    //    services.push_back(service);

    //    IRIList supportedServices = om.filterSupportedModels(combination, services);
    //    BOOST_REQUIRE_MESSAGE(supportedServices.size() == 1, "Sherpa support StereoImagerProvider");
    //}
}


BOOST_AUTO_TEST_CASE(resource_support_vector)
{
    {
        ResourceSupportVector nullSupport;
        BOOST_REQUIRE_MESSAGE(nullSupport.isNull(), "ResourceSupportVector is null");
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

        ResourceSupportVector intersection = ResourceSupportVector::intersection(a,b);

        BOOST_REQUIRE_MESSAGE(intersection(0) == 1, "Dim 0 --> 1");
        BOOST_REQUIRE_MESSAGE(intersection(1) == 1, "Dim 1 --> 1");
        BOOST_REQUIRE_MESSAGE(intersection(2) == 0, "Dim 1 --> 0");

        BOOST_REQUIRE_MESSAGE(a.contains(b), "ResourceSupportVector a contains b");
        BOOST_REQUIRE_MESSAGE(!b.contains(a), "ResourceSupportVector b does not contain a");

        {
            ResourceSupportVector missing = a.missingSupportFrom(b);
            BOOST_REQUIRE_MESSAGE(missing(2) == 1, "Missing support in dimension 2: " << missing(2));
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

        ResourceSupportVector intersection = ResourceSupportVector::intersection(a,b);

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
        BOOST_REQUIRE_MESSAGE(rSet == aSet, "Composition with model pool and empty set results in first argument");
    }
    {
        ModelPool::Set aSet;
        aSet.insert(a);
        ModelPool::Set rSet = Algebra::maxCompositions(empty,a);
        BOOST_REQUIRE_MESSAGE(rSet == aSet, "Composition with empty set and model pool results in second argument");
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
        ModelPool::Set cSet = Algebra::maxCompositions(a,b);
        BOOST_REQUIRE_MESSAGE(cSet.size() == 1, "1 composition expected: was " << cSet.size());
        ModelPool c = *cSet.begin();
        BOOST_REQUIRE_MESSAGE(c["http://model#a"] == 2, "Composition entry a->2 expected, was a->" << c["http://model#a"]);
        BOOST_REQUIRE_MESSAGE(c["http://model#b"] == 4, "Composition entry b->4 expected, was b->" << c["http://model#b"]);
        BOOST_REQUIRE_MESSAGE(c["http://model#c"] == 3, "Composition entry c->3 expected, was c->" << c["http://model#c"]);
    }
    {
        ModelPool::Set aSet;
        aSet.insert(a);

        ModelPool::Set bSet;
        bSet.insert(a);
        bSet.insert(b);
        ModelPool::Set cSet = Algebra::maxCompositions(aSet,bSet);
        BOOST_REQUIRE_MESSAGE(cSet.size() == 2, "2 compositions expected: was 2 " << cSet.size() << ModelPool::toString(cSet));

        ModelPool e0 = a;
        BOOST_REQUIRE_MESSAGE(cSet.find(e0) != cSet.end(), "Compositions contains 'a[+]a'");

        ModelPool e1;
        e1["http://model#a"] = 2;
        e1["http://model#b"] = 4;
        e1["http://model#c"] = 3;
        BOOST_REQUIRE_MESSAGE(cSet.find(e1) != cSet.end(), "Compositions contains 'a[+]b'");
    }
    // Identical set
    {
        ModelPool::Set bSet;
        bSet.insert(a);
        bSet.insert(b);
        ModelPool::Set cSet = Algebra::maxCompositions(bSet,bSet);
        BOOST_REQUIRE_MESSAGE(cSet.size() == 3, "3 compositions expected: was 3 " << cSet.size() << ModelPool::toString(cSet));

        ModelPool e0 = a;
        BOOST_REQUIRE_MESSAGE(cSet.find(e0) != cSet.end(), "Compositions contains 'a[+]a'");

        ModelPool e1;
        e1["http://model#a"] = 2;
        e1["http://model#b"] = 4;
        e1["http://model#c"] = 3;
        BOOST_REQUIRE_MESSAGE(cSet.find(e1) != cSet.end(), "Compositions contains 'a[+]b'");

        ModelPool e2 = b;
        BOOST_REQUIRE_MESSAGE(cSet.find(e1) != cSet.end(), "Compositions contains 'b[+]b'");
    }
}

BOOST_AUTO_TEST_CASE(connectivity)
{
    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()) );
    OrganizationModelAsk ask(om);

    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 2;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 10;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 20;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Payload")] = 40;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("CREX")] = 1;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );

    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("CREX")] = 2;

        BOOST_REQUIRE_MESSAGE( !Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );

    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE( !Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );

    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 1;
        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 2;
        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 20;
        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 1;
        modelPool[vocabulary::OM::resolve("CREX")] = 1;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 1;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 4;
        modelPool[vocabulary::OM::resolve("CREX")] = 3;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 10;
        modelPool[vocabulary::OM::resolve("CREX")] = 10;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("Sherpa")] = 18;
        modelPool[vocabulary::OM::resolve("CREX")] = 20;

        BOOST_REQUIRE_MESSAGE(Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
}

BOOST_AUTO_TEST_CASE(connectivity_multiple_interfaces)
{
    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-multiple-interfaces.owl") );
    OrganizationModelAsk ask(om);

    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 2;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask), "RobotA 2 should not be a valid combination: ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotB")] = 2;

        BOOST_REQUIRE_MESSAGE(!Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotC")] = 2;

        BOOST_REQUIRE_MESSAGE( !Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );

    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 1;
        modelPool[vocabulary::OM::resolve("RobotB")] = 1;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 1;
        modelPool[vocabulary::OM::resolve("RobotC")] = 1;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotB")] = 1;
        modelPool[vocabulary::OM::resolve("RobotC")] = 1;

        BOOST_REQUIRE_MESSAGE( !Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
    {
        ModelPool modelPool;
        modelPool[vocabulary::OM::resolve("RobotA")] = 1;
        modelPool[vocabulary::OM::resolve("RobotB")] = 1;
        modelPool[vocabulary::OM::resolve("RobotC")] = 1;

        BOOST_REQUIRE_MESSAGE( Connectivity::isFeasible(modelPool, ask), "ModelPool: " << modelPool.toString() );
    }
}

BOOST_AUTO_TEST_SUITE_END()
