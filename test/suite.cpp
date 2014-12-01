// Do NOT add anything to this file
// This header from boost takes ages to compile, so we make sure it is compiled
// only once (here)
//#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

std::string rdfTestFiles[] = { "test/data/om-schema-v0.1.owl", "test/data/om-schema-v0.2.owl", "test/data/om-schema-v0.5.owl" };

std::string getRootDir()
{
    char buffer[1024];
    BOOST_REQUIRE_MESSAGE( readlink("/proc/self/exe", buffer, 1024) != -1, "Retrieving current execution path");
    std::string str(buffer);
    std::string executionDir = str.substr(0, str.rfind("owl_om/"));
    std::string configurationPath = executionDir + "owl_om/";
    return configurationPath;
}

