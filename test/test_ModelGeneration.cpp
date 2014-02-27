#include <boost/test/unit_test.hpp>
#include <owl_om/gen/NamingConversion.hpp>
#include <owl_om/gen/ModelGenerator.hpp>
#include "test_utils.hpp"

using namespace owl_om;

std::string owlTestFiles[] = { "test/data/om-schema-v0.1.owl" };

BOOST_AUTO_TEST_CASE(naming_conversion)
{
    {
        CanonizedName name = NamingConversion::canonizeClassName("http://www.w3.org/test/blue/data#basename");
        BOOST_REQUIRE(name.getBasename() == "Basename");
        BOOST_REQUIRE_MESSAGE(name.getNamespace() == "w3::data", "NS: " << name.getNamespace());
    }
    {
        CanonizedName name = NamingConversion::canonizeClassName("http://w3.org/testa/blue/data#basename");
        BOOST_REQUIRE(name.getBasename() == "Basename");
        BOOST_REQUIRE_MESSAGE(name.getNamespace() == "w3::data", "NS: " << name.getNamespace());
    }
    {
        CanonizedName name = NamingConversion::canonizeClassName("http://w3.org/testa/blue/data");
        BOOST_REQUIRE(name.getBasename() == "Data");
        BOOST_REQUIRE_MESSAGE(name.getNamespace() == "w3", "NS: " << name.getNamespace());
    }
    {
        CanonizedName name = NamingConversion::canonizeClassName("http://www.rock-robotics.org/testa/blue/2012/om-schema#data");
        BOOST_REQUIRE_MESSAGE(name.getBasename() == "Data", name.getBasename());
        BOOST_REQUIRE_MESSAGE(name.getNamespace() == "rock_robotics::om_schema", "NS: " << name.getNamespace());
    }

    std::string snake = NamingConversion::snakeCase("BlaCase") ;
    BOOST_REQUIRE_MESSAGE( snake == "bla_case", "Snake case conversion " << snake);

    std::string camelCase = NamingConversion::camelCase("bla_case") ;
    BOOST_REQUIRE_MESSAGE( camelCase == "BlaCase", "Snake case conversion " << camelCase);
}

//BOOST_AUTO_TEST_CASE(test_rendering)
//{
//    using namespace owl_om;
//    Class::Ptr base(new Class("Base"));
//    Class::Ptr derived(new Class("Derived"));
//
//    derived->addSuperClass(base);
//
//    RenderedFiles renderedFiles = ModelGenerator::render(getRootDir() + "src/gen/templates/class.tpl", derived, "rock_robotics");
//    BOOST_TEST_MESSAGE(renderedFiles[0].content);
//
//    ModelGenerator::registerTemplate("class-hpp", getRootDir() + "src/gen/templates/class.tpl");
//    ModelGenerator generator(getRootDir() + owlTestFiles[0], "robot_robotics");
//    RenderedFiles files = generator.renderClassFiles();
//    RenderedFiles::const_iterator cit = files.begin();
//
//    for(; cit != files.end(); ++cit)
//    {
//        BOOST_TEST_MESSAGE("File: " << cit->filename << "\n" << cit->content);
//    }
//}
