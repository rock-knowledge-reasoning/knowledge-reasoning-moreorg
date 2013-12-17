#include <boost/test/unit_test.hpp>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Registry.hpp>

using namespace owl_om;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    Class::Ptr test(new Class("http://www.rock-robotics.org/2013/09/om-schmema#Test"));

    std::string klassname = test->getName();

    Registry registry;
    registry.registerClass(klassname, test);
    Class::Ptr retrievedClass = registry.getClass(klassname);

    BOOST_REQUIRE_MESSAGE( retrievedClass->getName() == test->getName(), "Registered class: expected '" << test->getName() << "' was '" << retrievedClass->getName() << "'");
    BOOST_REQUIRE_MESSAGE( retrievedClass->subclassOf( vocabulary::OWL::Thing() ) , "Registered class subclasses owl::Thing");

    BOOST_TEST_MESSAGE(registry.toString());
}
