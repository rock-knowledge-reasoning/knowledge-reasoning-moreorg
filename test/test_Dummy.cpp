#include <boost/test/unit_test.hpp>
#include <owl_om/Dummy.hpp>

using namespace owl_om;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    owl_om::DummyClass dummy;
    dummy.welcome();
}
