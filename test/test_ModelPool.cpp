#include <boost/test/unit_test.hpp>
#include <organization_model/ModelPool.hpp>
#include <organization_model/Algebra.hpp>

using namespace organization_model;

BOOST_AUTO_TEST_SUITE(model_pool)

BOOST_AUTO_TEST_CASE(equals)
{
    ModelPool a;
    a["a"] = 1;
    a["b"] = 10;
    a["c"] = 3;

    ModelPool b;
    b["b"] = a["b"];
    b["a"] = a["a"];
    b["c"] = a["c"];

    BOOST_REQUIRE_MESSAGE(a == b, "ModelPools are the same");
}

BOOST_AUTO_TEST_SUITE_END()
