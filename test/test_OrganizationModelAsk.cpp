#include <boost/test/unit_test.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <organization_model/OrganizationModelAsk.hpp>

#include "test_utils.hpp"

using namespace organization_model;

BOOST_AUTO_TEST_SUITE(organization_model_ask)

BOOST_AUTO_TEST_CASE(functional_saturation)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-schema-v0.8.owl"));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    OrganizationModelAsk ask(om);
    {

        IRI sherpa = OM::resolve("Sherpa");
        IRI payload = OM::resolve("Payload");
        IRI payloadCamera = OM::resolve("PayloadCamera");

        IRI stereoImageProvider = OM::resolve("StereoImageProvider");

        Service service(stereoImageProvider);
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationPoint(service, sherpa);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for StereoImageProvider: was " << saturationPoint);
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationPoint(service, payload);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 0, "0 Payload sufficient for StereoImageProvider: was " << saturationPoint);
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationPoint(service, payloadCamera);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 2, "2 PayloadCamera sufficient for StereoImageProvider: was" << saturationPoint);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
