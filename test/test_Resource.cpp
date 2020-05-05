#include <boost/test/unit_test.hpp>
#include <moreorg/Resource.hpp>

using namespace moreorg;
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

BOOST_AUTO_TEST_CASE(merge_single_set)
{
    owlapi::model::IRI modelA("http://test/a");
    owlapi::model::IRI modelB("http://test/b");
    owlapi::model::IRI propertyA("http://test/propertyA");

    Resource a0(modelA);
    PropertyConstraint pcA0(propertyA, PropertyConstraint::GREATER_EQUAL, 3);
    a0.addPropertyConstraint(pcA0);

    Resource a1(modelA);
    PropertyConstraint pcA1(propertyA, PropertyConstraint::GREATER_EQUAL, 4);
    a1.addPropertyConstraint(pcA1);

    Resource::Set resources = { a0, a1 };
    Resource::Set mergedResources = Resource::merge(resources);
    BOOST_REQUIRE_MESSAGE(mergedResources.size() == 1, "Merged resources of size"
            " 1");

    PropertyConstraint::Set constraints = mergedResources.begin()->getPropertyConstraints();
    BOOST_REQUIRE_MESSAGE(constraints.size() == 2, "There should be two"
            "constraints, was " << constraints.size());
    PropertyConstraint::Set expectedConstraints = { pcA1, pcA0 };
    BOOST_REQUIRE_MESSAGE(constraints == expectedConstraints, "Property constraint should equal "
            << PropertyConstraint::toString(expectedConstraints, 4)
            << " was "
            << PropertyConstraint::toString(constraints, 4));
}

BOOST_AUTO_TEST_CASE(merge_single_resource)
{
    owlapi::model::IRI modelA("http://test/a");
    owlapi::model::IRI propertyA("http://test/propertyA");

    Resource a0(modelA);

    Resource a1(modelA);
    PropertyConstraint pcA1(propertyA, PropertyConstraint::GREATER_EQUAL, 4);
    a1.addPropertyConstraint(pcA1);

    Resource::Set resources = { a0, a1 };
    Resource::Set mergedResources = Resource::merge(resources);
    BOOST_REQUIRE_MESSAGE(mergedResources.size() == 1, "There should be only"
            " one resource");

    PropertyConstraint::Set constraints = mergedResources.begin()->getPropertyConstraints();
    BOOST_REQUIRE_MESSAGE(constraints.size() == 1, "There should be one constraint");
    PropertyConstraint constraint = *constraints.begin();
    BOOST_REQUIRE_MESSAGE(constraint == pcA1, "Property constraint should equal "
            << pcA1.toString() << ", was " << constraint.toString());
}

BOOST_AUTO_TEST_SUITE_END()
