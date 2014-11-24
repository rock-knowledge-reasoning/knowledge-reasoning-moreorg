#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/OWLApi.hpp>
#include <owl_om/Vocabulary.hpp>
#include <boost/regex.hpp>

using namespace owlapi::model;

BOOST_AUTO_TEST_CASE(it_should_handle_iris)
{
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

    {
        IRI iri("http://www.w3.org/2002/07/owl#TransitiveProperty");
        URI uri = iri.toURI();
        BOOST_TEST_MESSAGE("URI should be same after toString 1: " << uri.toString());
        BOOST_TEST_MESSAGE("URI should be same after toString 2: " << uri.toString());
        BOOST_TEST_MESSAGE("URI should be same after toString 3: " << uri.toString());

        BOOST_REQUIRE_MESSAGE(iri.toString() == uri.toString(), "IRI should be same after uri conversion: " << iri.toString() << " vs. iri->uri->iri " << uri.toString() );
    }
    {
        URI relativeUri("Transitive");
        BOOST_REQUIRE_MESSAGE(relativeUri.toString() == "Transitive", "Relative uri remains the same " << relativeUri.toString());
        URI uri("http://www.w3.org/2002/07/owl#");
        BOOST_TEST_MESSAGE("Test base uri: " << uri.toString());

        URI resolvedUri = uri.resolve(relativeUri);

        BOOST_REQUIRE_MESSAGE(resolvedUri.toString() == "http://www.w3.org/2002/07/owl#Transitive", "Resolved uri via relative: '" << resolvedUri.toString() << "'");
    }

    {
        IRI iri("http://www.w3.org/2002/07/owl#");

        IRI resolvedIri = iri.resolve("TransitiveProperty");
        IRI expected("http://www.w3.org/2002/07/owl#TransitiveProperty");
 
        BOOST_REQUIRE_MESSAGE(resolvedIri == expected, "Resolved iri: " << resolvedIri << " vs. expected " << expected);

    }

    {
        IRI iri("http://www.w3.org/2002/07/owl#One+Two+Three[12]*");
        IRI iriFail("http://www.w3.org/2002/07/owl#One+Two+Three[]*");
        std::string escapedIri = iri.toEscapedString();

        boost::regex r(escapedIri);
        BOOST_REQUIRE_MESSAGE(regex_match(iri.toString(), r), "IRI correctly escaped -- positive test: regex is: " << r.str());
        BOOST_REQUIRE_MESSAGE(!regex_match(iriFail.toString(), r), "IRI correctly escaped -- negative test");
    }
}

BOOST_AUTO_TEST_CASE(it_should_handle_property_expressions)
{
    OWLObjectPropertyExpression::Ptr oProperty( new OWLObjectProperty("http://www.w3.org/2002/07/custom#has"));
    OWLInverseObjectProperty inverseProperty(oProperty);

    BOOST_REQUIRE_MESSAGE( inverseProperty.getInverse() == oProperty, "Inverse should be the same");
}

BOOST_AUTO_TEST_CASE(it_should_handle_class_expressions)
{
    using namespace owlapi::model;
    OWLClass baseClass("baseClass");

    IRI has("has");
    OWLObjectProperty::Ptr oPropertyPtr(new OWLObjectProperty(has));
    uint32_t cardinality = 10;
    OWLObjectExactCardinality oe(oPropertyPtr, cardinality, baseClass.getIRI());

    BOOST_REQUIRE( oe.getCardinality() == cardinality );
    BOOST_REQUIRE( oe.getCardinalityRestrictionType() == OWLCardinalityRestriction::EXACT );
    BOOST_REQUIRE( oe.isQualified() );
    BOOST_REQUIRE( oe.getQualification() == baseClass.getIRI() );
}
