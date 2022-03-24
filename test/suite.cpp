// Do NOT add anything to this file
// This header from boost takes ages to compile, so we make sure it is compiled
// only once (here)
//#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>

std::string rdfTestFiles[] = {"test/data/om-schema-v0.1.owl",
                              "test/data/om-schema-v0.2.owl",
                              "test/data/om-schema-v0.5.owl"};

std::string getRootDir()
{
    char buffer[1024];
    BOOST_REQUIRE_MESSAGE(readlink("/proc/self/exe", buffer, 1024) != -1,
                          "Retrieving current execution path");
    std::string str(buffer);
    std::vector<std::string> foldernames = {"organization_model", "moreorg"};
    std::string mainfolder;
    size_t textPos;
    for(const std::string& foldername : foldernames)
    {
        size_t pos = str.rfind(foldername + "/");
        if(pos != std::string::npos)
        {
            textPos = pos;
            mainfolder = foldername;
            break;
        }
    }

    std::string executionDir = str.substr(0, textPos);
    std::string configurationPath = executionDir + mainfolder + "/";
    return configurationPath;
}

std::string getOMSchema()
{
    return getRootDir() + "test/data/om-schema-latest.owl";
}
