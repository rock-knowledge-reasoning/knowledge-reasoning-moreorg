#include <boost/test/unit_test.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/reasoning/ResourceMatch.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"

using namespace organization_model;
using namespace organization_model::reasoning;
using namespace organization_model::vocabulary;

BOOST_AUTO_TEST_SUITE(organization_model_ask)

BOOST_AUTO_TEST_CASE(recursive_resolution)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = OM::resolve("Sherpa");
    IRI basecamp = OM::resolve("BaseCamp");
    IRI locationImageProvider = OM::resolve("LocationImageProvider");

    OrganizationModelAsk ask(om);
    Functionality functionality(locationImageProvider);

    typedef std::pair<uint32_t, algebra::SupportType> ExpectedResult;
    std::vector<ExpectedResult> expected;
    IRIList agents;

    agents.push_back(sherpa);
    expected.push_back( ExpectedResult(1,algebra::FULL_SUPPORT) );

    agents.push_back(basecamp);
    expected.push_back( ExpectedResult(0, algebra::NO_SUPPORT) );


    for(int i = 0; i < agents.size(); ++i)
    {
        IRI agent = agents[i];
        IRIList combination;
        combination.push_back(agent);

        IRIList functionalities;
        functionalities.push_back(functionality.getModel());

        IRIList supportedFunctionalityModels = ResourceMatch::filterSupportedModels(combination, functionalities, om->ontology());

        OWLOntologyAsk oAsk(om->ontology());
        std::vector<OWLCardinalityRestriction::Ptr> restrictions = oAsk.getCardinalityRestrictions(locationImageProvider);
        BOOST_TEST_MESSAGE("Cardinality: " << OWLCardinalityRestriction::toString(restrictions));
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality.getModel(), agent);
            BOOST_REQUIRE_MESSAGE(saturationPoint == expected[i].first,  expected[i].first << " expected as saturation point for " << agent.getFragment() << " sufficient for LocationImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, agent, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == expected[i].second, "Support from " << agent.getFragment() << " for LocationImageProvider at saturation point: expected " << expected[i].second << " was " << supportType);
        }
    }
}

BOOST_AUTO_TEST_CASE(functional_saturation)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    IRI stereoImageProvider = OM::resolve("StereoImageProvider");
    IRI locationImageProvider = OM::resolve("LocationImageProvider");

    OrganizationModelAsk ask(om);
    {
        IRI functionality = stereoImageProvider;
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, sherpa);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, sherpa, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from sherpa for StereoImageProvider at saturation point");
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, payload);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 0, "0 Payload sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, payload, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::NO_SUPPORT, "No support from payload for StereoImageProvider");
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, payloadCamera);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 2, "2 PayloadCamera sufficient for StereoImageProvider: was" << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, payloadCamera, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from payload camera for StereoImageProvider at saturation point");
        }
    }
    {
        IRI functionality = locationImageProvider;
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, sherpa);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, sherpa, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from sherpa for StereoImageProvider at saturation point");
        }
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[crex] = 1;

        ask.prepare(modelPool);

        FunctionalitySet functionalitySet;
        Functionality functionality(stereoImageProvider);
        functionalitySet.insert(functionality);

        ModelPoolSet combinations = ask.getBoundedResourceSupport(functionalitySet);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Bounded resource support for: " << stereoImageProvider.toString() << " by '" << ModelPool::toString(combinations) << "'");
    }
}

BOOST_AUTO_TEST_CASE(get_resource_support)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;

        OrganizationModelAsk ask(om, modelPool);
        IRI functionalityModel = OM::resolve("ImageProvider");
        FunctionalitySet functionalities;
        functionalities.insert( Functionality(functionalityModel) );

        ModelPoolSet combinations = ask.getResourceSupport(functionalities);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting : " << functionalityModel.toString() << ": '" << ModelPool::toString(combinations) << "'");
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[crex] = 1;

        OrganizationModelAsk ask(om, modelPool);
        FunctionalitySet functionalities;

        IRI imageProvider = OM::resolve("ImageProvider");
        IRI emiPowerProvider = OM::resolve("EmiPowerProvider");
        functionalities.insert( Functionality(imageProvider) );
        functionalities.insert( Functionality(emiPowerProvider) );

        ModelPoolSet combinations = ask.getResourceSupport(functionalities);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting : " << imageProvider.toString() << " and " << emiPowerProvider.toString() << ": '" << ModelPool::toString(combinations) << "'");
    }
}

BOOST_AUTO_TEST_CASE(apply_upper_bound)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    // upperBound
    ModelPool modelPool;
    modelPool[sherpa] = 1;
    modelPool[crex] = 1;

    OrganizationModelAsk ask(om, modelPool);

    ModelCombinationSet combinations;
    {
        IRIList combination;
        combination.push_back(sherpa);
        combinations.insert(combination);
    }
    {
        IRIList combination;
        combination.push_back(sherpa);
        combination.push_back(crex);
        combinations.insert(combination);
    }
    {
        IRIList combination;
        combination.push_back(crex);
        combination.push_back(crex);
        combination.push_back(crex);
        combinations.insert(combination);
    }

    ModelCombinationSet boundedSet = ask.applyUpperBound(combinations, modelPool);
    BOOST_REQUIRE_MESSAGE(boundedSet.size() == 2, "BoundedSet: expected size: 2 was " << boundedSet.size() << ": " << OrganizationModel::toString(boundedSet) );
}

BOOST_AUTO_TEST_CASE(to_string)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    ModelPool pool;
    pool.setResourceCount( OM::resolve("Sherpa"), 3);
    pool.setResourceCount( OM::resolve("CREX"), 2);
    pool.setResourceCount( OM::resolve("CoyoteIII"), 3);
    pool.setResourceCount( OM::resolve("BaseCamp"), 5);
    pool.setResourceCount( OM::resolve("Payload"), 10);

    bool applyFunctionalSaturationBound = true;
    OrganizationModelAsk ask(om, pool, applyFunctionalSaturationBound);
    BOOST_TEST_MESSAGE(ask.toString());
}

BOOST_AUTO_TEST_CASE(transport)
{
}

BOOST_AUTO_TEST_SUITE_END()
