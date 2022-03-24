#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <moreorg/OrganizationModel.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/PropertyConstraintSolver.hpp>
#include <moreorg/facades/Robot.hpp>
#include <moreorg/vocabularies/OM.hpp>

using namespace moreorg;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(property_constraint_solver)

BOOST_AUTO_TEST_CASE(value_bound)
{
    owlapi::model::IRI propertyA("http://test/propertyA");
    PropertyConstraint pcA0(propertyA, PropertyConstraint::GREATER_EQUAL, 3.0);
    PropertyConstraint pcA1(propertyA, PropertyConstraint::LESS_THAN, 10.0);
    PropertyConstraint pcA2(propertyA, PropertyConstraint::LESS_THAN, 8.5);
    PropertyConstraint pcA3(propertyA, PropertyConstraint::LESS_THAN, 2.5);

    {
        PropertyConstraint::List constraints = {pcA0, pcA1, pcA2};
        ValueBound vb = PropertyConstraintSolver::merge(constraints);

        BOOST_REQUIRE_MESSAGE(vb.getMin() == 3.0,
                              "Min is set to " << vb.getMin() << " expected "
                                               << 3.0);
        BOOST_REQUIRE_MESSAGE(vb.getMax() == 8.5,
                              "Max is set to " << vb.getMax() << " expected "
                                               << 8.5);
    }
    {
        PropertyConstraint::List constraints = {pcA0, pcA1, pcA2, pcA3};
        BOOST_REQUIRE_THROW(PropertyConstraintSolver::merge(constraints),
                            std::invalid_argument);
    }
}

BOOST_AUTO_TEST_CASE(fulfillment)
{
    // std::string filename = "" + getRootDir() +
    //    "/test/data/om-robot-sherpa-tt.ttl";
    std::string filename = "" + getRootDir() + "/test/data/test-load-area.ttl";
    OrganizationModel::Ptr organizationModel =
        make_shared<OrganizationModel>(filename);

    owlapi::model::IRI sherpaTT = vocabulary::OM::resolve("SherpaTT");

    ModelPool pool;
    pool[sherpaTT] = 1;

    OrganizationModelAsk ask(organizationModel, pool, true);
    facades::Robot robot = facades::Robot::getInstance(pool, ask);

    BOOST_REQUIRE_MESSAGE(robot.getPowerSourceCapacity() != 0,
                          "SherpaTT has "
                          " power source capacity");

    BOOST_REQUIRE_MESSAGE(robot.getLoadArea() == 0.15 * 0.15,
                          "SherpaTT has load area");
    {
        PropertyConstraint massC0(vocabulary::OM::mass(),
                                  PropertyConstraint::GREATER_EQUAL,
                                  100.0);
        BOOST_REQUIRE_MESSAGE(!massC0.usesPropertyReference(),
                              "Is not a self referencing constraint");
        BOOST_REQUIRE_MESSAGE(massC0.getReferenceValue(robot) == 100.0,
                              "Property reference mass");

        Fulfillment f =
            PropertyConstraintSolver::fulfills(robot,
                                               PropertyConstraint::Set{massC0});
        BOOST_REQUIRE_MESSAGE(f.isMet(),
                              "SherpaTT has a mass greater than 100.0 kg");
    }

    {
        PropertyConstraint massC1(vocabulary::OM::mass(),
                                  PropertyConstraint::LESS_THAN,
                                  50.0);
        Fulfillment f = PropertyConstraintSolver::fulfills(
            robot,
            PropertyConstraint::List{massC1});
        BOOST_REQUIRE_MESSAGE(!f.isMet(),
                              "SherpaTT has not a mass less than 50.0 kg");
    }

    {
        PropertyConstraint massC1(vocabulary::OM::resolve("loadAreaSize"),
                                  PropertyConstraint::GREATER_THAN,
                                  0.1);
        Fulfillment f = PropertyConstraintSolver::fulfills(
            robot,
            PropertyConstraint::List{massC1});
        BOOST_REQUIRE_MESSAGE(!f.isMet(),
                              "SherpaTT has a load area > 0.1 cm^2");
    }
}

BOOST_AUTO_TEST_SUITE_END()
