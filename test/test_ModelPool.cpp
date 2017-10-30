#include <boost/test/unit_test.hpp>
#include <organization_model/ModelPool.hpp>
#include <organization_model/ModelPoolIterator.hpp>
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

BOOST_AUTO_TEST_CASE(iterator)
{
    ModelPool from;
    ModelPool to;
    ModelPool step;

    from["a"] = 1;
    to["a"] = 10;
    step["a"] = 1;

    {
        ModelPoolIterator iterator(from, to, step);
        int i = 0;
        while(iterator.next())
        {
            ++i;
            ModelPool pool = iterator.current();
            BOOST_TEST_MESSAGE(pool.toString());
        }
        BOOST_REQUIRE_MESSAGE(i == 10, "Iterations expected: 10 but counted '" << i << "'");
    }

    step["a"] = 2;
    {
        ModelPoolIterator iterator(from, to, step);
        int i = 0;
        while(iterator.next())
        {
            ++i;
            ModelPool pool = iterator.current();
            BOOST_TEST_MESSAGE(pool.toString());
        }
        // 1 3 5 7 9 10
        BOOST_REQUIRE_MESSAGE(i == 6, "Iterations expected: 6, but counted '" << i << "'");
    }

    to["a"] = 3;
    from["b"] = 1;
    to["b"] = 3;
    step["a"] = 1;
    step["b"] = 1;
    {
        ModelPoolIterator iterator(from, to, step);
        int i = 0;
        while(iterator.next())
        {
            ++i;
            ModelPool pool = iterator.current();
            BOOST_TEST_MESSAGE(pool.toString());
        }
        // 1 3 5 7 9 10
    //    BOOST_REQUIRE_MESSAGE(i == 6, "Iterations expected: 6, but counted '" << i << "'");
    }

}

BOOST_AUTO_TEST_SUITE_END()
