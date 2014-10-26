#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/utils/IntegerPartitioning.hpp>

using namespace multiagent::utils;

BOOST_AUTO_TEST_CASE(it_should_compute_integer_partitions)
{

IntegerPartitioning ip;
ip.compute(16);

BOOST_TEST_MESSAGE(ip.toString());

}
