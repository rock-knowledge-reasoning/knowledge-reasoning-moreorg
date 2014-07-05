#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/OWLApi.hpp>
#include <owl_om/Vocabulary.hpp>

BOOST_AUTO_TEST_CASE(it_should_handle_iris)
{
    using namespace owlapi::model;
    {
        IRI iri;
        BOOST_REQUIRE_THROW(iri.getScheme(), std::invalid_argument);

        BOOST_REQUIRE_MESSAGE(iri.getPrefix().empty(), "Prefix should be empty");
        BOOST_REQUIRE_MESSAGE(iri.getPrefix() == iri.getNamespace(), "Prefix should be same as namespace");
        BOOST_REQUIRE_MESSAGE(iri.getRemainder().empty(), "Remainder should be empty");
    }

    {
        std::string prefix = "http://www.rock-robotics.org/2001/09/om-schema#";
        std::string suffix = "test/";

        IRI iri(prefix, suffix);
        BOOST_REQUIRE_MESSAGE(iri.getScheme() == "http", "Scheme should be http");
        BOOST_REQUIRE_MESSAGE(iri.getPrefix() == iri.getNamespace(), "Prefix should be same as namespace");

        BOOST_REQUIRE_MESSAGE(iri.getPrefix() == prefix, "Prefix expected '" << prefix << "' got '" << iri.getPrefix() << "'");
        BOOST_REQUIRE_MESSAGE(iri.getRemainder() == suffix, "Remainder expected '" << suffix << "' got '" << iri.getRemainder() << "'");
        BOOST_REQUIRE_MESSAGE(iri.getRemainder() == suffix, "Remainder expected '" << suffix << "' got '" << iri.getRemainder() << "'");

    }
    {
        IRI iri("http://bla/1/2/3/","");
        BOOST_REQUIRE_MESSAGE(iri.isAbsolute(), "IRI '" << iri << " is absolute");
        IRI otherIri = iri.resolve("../../../2012/om-schema#bla");
        BOOST_TEST_MESSAGE("Resolve " << otherIri);

        URI uri("/www.example.org/index.php");
        BOOST_REQUIRE_MESSAGE(uri.isAbsolute(), "Uri is absolute " << uri.toString());
    }

    {
        std::string prefix = "http://www.rock-robotics.org/";
        std::string suffix = "test";

        IRI iri(prefix, suffix);
        BOOST_REQUIRE_MESSAGE(iri.getScheme() == "http", "Scheme should be http");
        BOOST_REQUIRE_MESSAGE(iri.getPrefix() == iri.getNamespace(), "Prefix should be same as namespace");

        BOOST_REQUIRE_MESSAGE(iri.getPrefix() == prefix, "Prefix expected '" << prefix << "' got '" << iri.getPrefix() << "'");
        BOOST_REQUIRE_MESSAGE(iri.getRemainder() == suffix, "Remainder expected '" << suffix << "' got '" << iri.getRemainder() << "'");
    }


}
