#include <boost/test/unit_test.hpp>
#include <organization_model/algebra/ResourceSupportVector.hpp>

using namespace organization_model::algebra;

BOOST_AUTO_TEST_SUITE(algebra)

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
            double dos = a.degreeOfSupport(b);
            BOOST_REQUIRE_MESSAGE(dos < 1 && dos > 0, "Degree of support from b for a is < 1, but > 0: " << dos);

            ResourceSupportVector missing = a.missingSupportFrom(b);
            BOOST_REQUIRE_MESSAGE(missing(2) == 1, "Missing support in dimension 2: " << missing(2));
        }

        {
            double dos = b.degreeOfSupport(a);
            BOOST_REQUIRE_MESSAGE((dos - 1.0) >= -1E-06 , "Degree of support from b for a is >= 1 value is: " << dos);
            BOOST_CHECK_EQUAL(dos, 1.0);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
