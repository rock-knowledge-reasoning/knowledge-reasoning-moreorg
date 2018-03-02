#include <boost/test/unit_test.hpp>
#include <organization_model/Resource.hpp>

using namespace organization_model;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(resource)

BOOST_AUTO_TEST_CASE(merge)
{
    owlapi::model::IRI modelA("http://test/a");
    owlapi::model::IRI modelB("http://test/b");
    owlapi::model::IRI propertyA("http://test/propertyA");
    owlapi::model::IRI propertyB("http://test/propertyB");

    {
        Resource a(modelA);
        Resource b(modelB);

        BOOST_REQUIRE_THROW(a.merge(b), std::invalid_argument);
    }
    {
        Resource a(modelA);
        Resource b(modelA);

        BOOST_REQUIRE_NO_THROW(a.merge(b));
    }

    {
        Resource a0(modelA);
        PropertyConstraint pcA0(propertyA, PropertyConstraint::GREATER_EQUAL, 10);
        a0.addPropertyConstraint(pcA0);

        Resource a1(modelA);
        PropertyConstraint pcA1(propertyA, PropertyConstraint::GREATER_EQUAL, 20);
        a1.addPropertyConstraint(pcA1);

        a0.merge(a1);
        BOOST_REQUIRE_MESSAGE(a0.getPropertyConstraints().size() == 2, "Merge should result in 2 property constraints, but was " << a0.getPropertyConstraints().size());

        // Checkin sets
        Resource b0(modelB);

        Resource::Set aSet;
        aSet.insert(a0);
        aSet.insert(a1);

        Resource::Set bSet;
        bSet.insert(a0);
        bSet.insert(b0);

        Resource::Set r = Resource::merge(aSet, bSet);
        BOOST_REQUIRE_MESSAGE(r.size() == 2, "Merge should result in two functionalities, was " << r.size());
    }
}

BOOST_AUTO_TEST_SUITE_END()
