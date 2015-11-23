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
    IRI locationImageProvider = OM::resolve("LocationImageProvider");

    OrganizationModelAsk ask(om);
    Service service(locationImageProvider);

    IRIList combination;
    combination.push_back(sherpa);

    IRIList services;
    services.push_back(service.getModel());

    IRIList supportedServiceModels = ResourceMatch::filterSupportedModels(combination, services, om->ontology());

    OWLOntologyAsk oAsk(om->ontology());
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = oAsk.getCardinalityRestrictions(locationImageProvider);
    BOOST_TEST_MESSAGE("Cardinality: " << OWLCardinalityRestriction::toString(restrictions));
    BOOST_REQUIRE_MESSAGE(!supportedServiceModels.empty(), "Supported serviced models: " << services << " models where: " << supportedServiceModels);

    {
        uint32_t saturationPoint = ask.getFunctionalSaturationBound(service, sherpa);
        BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for LocationImageProvider: was " << saturationPoint);

        algebra::SupportType supportType = ask.getSupportType(service, sherpa, saturationPoint);
        BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from sherpa for LocationImageProvider at saturation point");
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
        Service service(stereoImageProvider);
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(service, sherpa);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(service, sherpa, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from sherpa for StereoImageProvider at saturation point");
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(service, payload);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 0, "0 Payload sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(service, payload, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::NO_SUPPORT, "No support from payload for StereoImageProvider");
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(service, payloadCamera);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 2, "2 PayloadCamera sufficient for StereoImageProvider: was" << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(service, payloadCamera, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from payload camera for StereoImageProvider at saturation point");
        }
    }
    {
        Service service(locationImageProvider);
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(service, sherpa);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(service, sherpa, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from sherpa for StereoImageProvider at saturation point");
        }
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[crex] = 1;

        ask.prepare(modelPool);

        ServiceSet serviceSet;
        Service service(stereoImageProvider);
        serviceSet.insert(service);

        ModelCombinationSet combinations = ask.getBoundedResourceSupport(serviceSet);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Bounded resource support for: " << stereoImageProvider.toString() << " by '" << OrganizationModel::toString(combinations) << "'");
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
        IRI serviceModel = OM::resolve("ImageProvider");
        ServiceSet services;
        services.insert( Service(serviceModel) );

        ModelCombinationSet combinations = ask.getResourceSupport(services);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting : " << serviceModel.toString() << ": '" << OrganizationModel::toString(combinations) << "'");
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[crex] = 1;

        OrganizationModelAsk ask(om, modelPool);
        ServiceSet services;

        IRI imageProvider = OM::resolve("ImageProvider");
        IRI emiPowerProvider = OM::resolve("EmiPowerProvider");
        services.insert( Service(imageProvider) );
        services.insert( Service(emiPowerProvider) );

        ModelCombinationSet combinations = ask.getResourceSupport(services);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting : " << imageProvider.toString() << " and " << emiPowerProvider.toString() << ": '" << OrganizationModel::toString(combinations) << "'");
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
