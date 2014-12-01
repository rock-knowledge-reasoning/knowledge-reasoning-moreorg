#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/Ontology.hpp>

using namespace owl_om;

BOOST_AUTO_TEST_CASE(it_should_load_restrictions)
{
    Ontology::Ptr ontology = Ontology::fromFile( getRootDir() + "/test/data/om-schema-v0.5.owl");

}
