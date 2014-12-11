#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/owlapi/model/OWLOntology.hpp>
#include <owl_om/owlapi/model/OWLOntologyReader.hpp>
#include <owl_om/owlapi/model/OWLOntologyAsk.hpp>

using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(ontology)

BOOST_AUTO_TEST_CASE(it_should_load_restrictions)
{
    OWLOntologyReader reader;
    OWLOntology::Ptr ontology = reader.fromFile( getRootDir() + "/test/data/om-schema-v0.6.owl");
    OWLOntologyAsk ask(ontology);

    {
        std::vector<IRI> klasses;
        IRI sherpa("http://www.rock-robotics.org/2014/01/om-schema#Sherpa");
        klasses.push_back(sherpa);
        std::map<IRI, std::vector<OWLCardinalityRestriction::Ptr> > restrictionsMap = ask.getCardinalityRestrictions(klasses);
        BOOST_REQUIRE(restrictionsMap.size() > 0);

        std::vector<OWLCardinalityRestriction::Ptr> restrictions = restrictionsMap[sherpa];
        BOOST_REQUIRE(restrictions.size() > 0);

        std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = restrictions.begin();
        for(; cit != restrictions.end(); ++cit)
        {
            BOOST_TEST_MESSAGE("Restriction: " << (*cit)->toString());
        }
    }
    {
        IRI iri("http://www.rock-robotics.org/2014/01/om-schema#PayloadCamera");
        std::vector<OWLCardinalityRestriction::Ptr> restrictions = ask.getCardinalityRestrictions(iri);
        BOOST_CHECK_MESSAGE(restrictions.size() > 2, "PayloadCamera should inherit restrictions");

        std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = restrictions.begin();
        for(; cit != restrictions.end(); ++cit)
        {
            BOOST_TEST_MESSAGE("Restrictions: " << (*cit)->toString());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
