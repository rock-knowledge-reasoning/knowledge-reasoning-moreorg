#include <boost/test/unit_test.hpp>
#include <base/Time.hpp>
#include <owlapi/io/OWLOntologyIO.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/OWLApi.hpp>
#include <organization_model/reasoning/ResourceMatch.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include <organization_model/OrganizationModelAsk.hpp>

#include "test_utils.hpp"

using namespace owlapi;
using namespace owlapi::model;
using namespace organization_model;
using namespace organization_model::reasoning;

BOOST_AUTO_TEST_SUITE(csp)

//BOOST_AUTO_TEST_CASE(match_resources)
//{
//    //OWLOntology::Ptr ontology(new OWLOntology());
//    OWLOntology::Ptr ontology = io::OWLOntologyIO::fromFile( getRootDir() + "/test/data/om-schema-v0.6.owl");
//    ontology->refresh();
//
//    OWLOntologyTell tell(ontology);
//    OWLOntologyAsk ask(ontology);
//    tell.initializeDefaultClasses();
//
//
//    OWLClass::Ptr a = tell.klass("http://klass/base");
//    OWLClass::Ptr b = tell.klass("http://klass/base-derived");
//    OWLClass::Ptr c = tell.klass("http://klass/base-derived-derived");
//
//    tell.subclassOf(c,b);
//    tell.subclassOf(b,a);
//    ontology->refresh();
//    BOOST_REQUIRE_MESSAGE(ask.isSubClassOf(c->getIRI(), b->getIRI()), "C should be subclass of b");
//
//    IRIList query, resourcePool;
//
//    query.push_back(a->getIRI());
//    query.push_back(c->getIRI());
//
//    resourcePool.push_back(b->getIRI());
//    resourcePool.push_back(c->getIRI());
//
//    ResourceMatch* match = ResourceMatch::solve(query, resourcePool, ontology);
//
//    BOOST_TEST_MESSAGE("Assignment: " << match->toString());
//
//    IRI assignment = match->getAssignment(a->getIRI());
//    BOOST_REQUIRE_MESSAGE( assignment == b->getIRI(), "Expected base to be matched by base-derived");
//}

BOOST_AUTO_TEST_CASE(match_resource_via_restrictions)
{
    OWLOntology::Ptr ontology(new OWLOntology());
    OWLOntologyTell tell(ontology);
    OWLOntologyAsk ask(ontology);
    tell.initializeDefaultClasses();

    OWLClass::Ptr a = tell.klass("http://klass/base");
    OWLClass::Ptr b = tell.klass("http://klass/base-derived");
    OWLClass::Ptr c = tell.klass("http://klass/base-derived-derived");
    OWLObjectProperty::Ptr property = tell.objectProperty("http://property/has");

    tell.subClassOf(c,b);
    tell.subClassOf(b,a);
    ontology->refresh();
    BOOST_REQUIRE_MESSAGE(ask.isSubClassOf(c->getIRI(), b->getIRI()), "C should be subclass of b");

    std::vector<OWLCardinalityRestriction::Ptr> query, resourcePool;
    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 2, a->getIRI()));
        query.push_back(restriction);
    }
    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 2, c->getIRI()));
        query.push_back(restriction);
    }

    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 3, b->getIRI()));
        resourcePool.push_back(restriction);
    }
    {
        OWLCardinalityRestriction::Ptr restriction(new OWLExactCardinalityRestriction(property, 3, c->getIRI()));
        resourcePool.push_back(restriction);
    }

    using namespace organization_model::reasoning;
    ModelBound::List required = ResourceMatch::toModelBoundList(query);
    ModelBound::List available = ResourceMatch::toModelBoundList(resourcePool);

    ResourceMatch::Solution solution = ResourceMatch::solve(query, resourcePool, ontology);
    BOOST_TEST_MESSAGE("Solution:" << solution.toString());
}

BOOST_AUTO_TEST_CASE(provider_via_restrictions)
{
    OWLOntology::Ptr ontology = io::OWLOntologyIO::fromFile( getOMSchema() );
    ontology->refresh();

    OWLOntologyTell tell(ontology);
    OWLOntologyAsk ask(ontology);
    tell.initializeDefaultClasses();

    IRI sherpa = organization_model::vocabulary::OM::resolve("Sherpa");
    IRI move_to = organization_model::vocabulary::OM::resolve("MoveTo");
    IRI image_provider = organization_model::vocabulary::OM::resolve("ImageProvider");
    IRI stereo_image_provider = organization_model::vocabulary::OM::resolve("StereoImageProvider");
    IRI location_image_provider = organization_model::vocabulary::OM::resolve("LocationImageProvider");

    std::vector<OWLCardinalityRestriction::Ptr> r_sherpa = ask.getCardinalityRestrictions(sherpa);
    std::vector<OWLCardinalityRestriction::Ptr> r_move_to = ask.getCardinalityRestrictions(move_to);
    std::vector<OWLCardinalityRestriction::Ptr> r_image_provider = ask.getCardinalityRestrictions(image_provider);
    std::vector<OWLCardinalityRestriction::Ptr> r_stereo_image_provider = ask.getCardinalityRestrictions(stereo_image_provider);
    std::vector<OWLCardinalityRestriction::Ptr> r_location_image_provider = ask.getCardinalityRestrictions(location_image_provider);

    using namespace organization_model::reasoning;
    ResourceMatch::Solution fulfillment = ResourceMatch::solve(r_move_to, r_sherpa, ontology);
    BOOST_TEST_MESSAGE("Sherpa provides MoveTo\nAssignment: " << fulfillment.toString());
    BOOST_TEST_MESSAGE("MoveTo requirements: " << OWLCardinalityRestriction::toString(r_move_to));
    BOOST_TEST_MESSAGE("Sherpa provides: " << OWLCardinalityRestriction::toString(r_sherpa));
    {
        std::string capabilityName = "Localization";
        ModelBound::List modelBounds = fulfillment.getAssignments(organization_model::vocabulary::OM::resolve(capabilityName));
        BOOST_REQUIRE_MESSAGE(modelBounds.size() == 1, "Matching of 1 provided resource expected, but got '" << modelBounds.size());

        ModelBound modelBound = modelBounds.front();
        BOOST_REQUIRE_MESSAGE(modelBound.min == 1 && modelBound.max == 1, "Model bound for '" << modelBound.model << "', expected min 1, max 1, but got: " << modelBound.toString());
    }
    {
        std::string capabilityName = "Locomotion";
        ModelBound::List modelBounds = fulfillment.getAssignments(organization_model::vocabulary::OM::resolve(capabilityName));
        BOOST_REQUIRE_MESSAGE(modelBounds.size() == 1, "Matching of 1 provided resource expected, but got '" << modelBounds.size());

        ModelBound modelBound = modelBounds.front();
        BOOST_REQUIRE_MESSAGE(modelBound.min == 1 && modelBound.max == 1, "Model bound for '" << modelBound.model << "', expected min 1, max 1, but got: " << modelBound.toString());
    }
    {
        std::string capabilityName = "Mapping";
        ModelBound::List modelBounds = fulfillment.getAssignments(organization_model::vocabulary::OM::resolve(capabilityName));
        BOOST_REQUIRE_MESSAGE(modelBounds.size() == 1, "Matching of 1 provided resource expected, but got '" << modelBounds.size());

        ModelBound modelBound = modelBounds.front();
        BOOST_REQUIRE_MESSAGE(modelBound.min == 1 && modelBound.max == 1, "Model bound for '" << modelBound.model << "', expected min 1, max 1, but got: " << modelBound.toString());
    }
    {
        std::string capabilityName = "Power";
        ModelBound::List modelBounds = fulfillment.getAssignments(organization_model::vocabulary::OM::resolve(capabilityName));
        BOOST_REQUIRE_MESSAGE(modelBounds.size() == 1, "Matching of 1 provided resource expected, but got '" << modelBounds.size());

        ModelBound modelBound = modelBounds.front();
        BOOST_REQUIRE_MESSAGE(modelBound.min == 1 && modelBound.max == 1, "Model bound for '" << modelBound.model << "', expected min 1, max 1, but got: " << modelBound.toString());
    }


    fulfillment = ResourceMatch::solve(r_image_provider, r_sherpa, ontology);
    BOOST_TEST_MESSAGE("Sherpa provides ImageProvider\nAssignment: " << fulfillment.toString());

    {
        OWLObjectProperty::Ptr hasProperty = ask.getOWLObjectProperty( organization_model::vocabulary::OM::resolve("has") );
        OWLCardinalityRestriction::Ptr restriction(new OWLObjectMinCardinality(hasProperty, 1, image_provider));
        tell.subClassOf(sherpa, restriction);
    }

    fulfillment = ResourceMatch::solve(r_stereo_image_provider, r_sherpa, ontology);
    BOOST_TEST_MESSAGE("Sherpa provides StereoImageProvider\nAssignment: " << fulfillment.toString());

    {
        OWLObjectProperty::Ptr hasProperty = ask.getOWLObjectProperty( organization_model::vocabulary::OM::resolve("has") );
        OWLCardinalityRestriction::Ptr restriction(new OWLObjectMinCardinality(hasProperty, 1, move_to));
        tell.subClassOf(sherpa, restriction);
    }

    // Second level of service provisioning -- requires other services to be
    // recognized: move_to and stereo
    //
    // 1. first infer 1. level services
    //
    // Constraints:
    //  subclassing constraints, should also allow to account for limiting
    //  facts, e.g. sherpa+crex -> enable, disable, but that will be important
    //  lateron
    //
    //
    //  TODO: add --> infer service -- mark_service "add provides", -->
    //  getCardinalityRestrictions(... , filter on qualification, e.g. 'has','provide')
    //
    //  restriction tree
    //  --> location_image_provider
    //    --> 1 - move_to
    //    --> 1 - image_provider
    std::vector<OWLCardinalityRestriction::Ptr> r_sherpa_with_service = ask.getCardinalityRestrictions(sherpa);
    try {
        fulfillment = ResourceMatch::solve(r_location_image_provider, r_sherpa_with_service, ontology);
        BOOST_REQUIRE_MESSAGE(true, "Sherpa provides LocationImageProvider\nAssignment: " << fulfillment.toString());
    } catch(...)
    {
        BOOST_TEST_MESSAGE("Sherpa does not provide LocationImageProvider\nAssignment failed for: " << location_image_provider);
        for(OWLCardinalityRestriction::Ptr r : r_sherpa_with_service)
        {
            BOOST_TEST_MESSAGE("Cardinality: " << r->toString());
        }
        BOOST_REQUIRE_MESSAGE(false, "Assignment failed");
    }

    if( ResourceMatch::isSupporting(sherpa, location_image_provider, ontology) )
    {
        BOOST_REQUIRE_MESSAGE(true, "Assignment successful");
    } else {
        BOOST_REQUIRE_MESSAGE(false, "Computing fulfills failed");
    }

    owlapi::model::IRIList combinedSystem;
    combinedSystem.push_back(sherpa);

    owlapi::model::IRIList serviceModels;
    // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#LocationImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider
    serviceModels.push_back(location_image_provider);
    serviceModels.push_back(image_provider);
    serviceModels.push_back(move_to);
    IRI emi_power_provider = organization_model::vocabulary::OM::resolve("EmiPowerProvider");
    serviceModels.push_back(emi_power_provider);

    {
        owlapi::model::IRIList supportedModels = ResourceMatch::filterSupportedModels(combinedSystem, serviceModels, ontology);
        BOOST_REQUIRE_MESSAGE(supportedModels.size() == 4, "Services supported by sherpa");
    }

    {
        base::Time startTime = base::Time::now();
        owlapi::model::IRIList supportedModels = ResourceMatch::filterSupportedModels(combinedSystem, serviceModels, ontology);
        base::Time stopTime = base::Time::now();
        BOOST_REQUIRE_MESSAGE(supportedModels.size() == 4, "Services supported by sherpa: computing time: " << (stopTime - startTime).toSeconds());
    }

}

BOOST_AUTO_TEST_CASE(performance_three_sherpa)
{
    OWLOntology::Ptr ontology = io::OWLOntologyIO::fromFile( getRootDir() + "/test/data/om-schema-v0.6.owl");
    ontology->refresh();

    OWLOntologyTell tell(ontology);
    OWLOntologyAsk ask(ontology);
    tell.initializeDefaultClasses();

    IRI sherpa = organization_model::vocabulary::OM::resolve("Sherpa");
    IRI move_to = organization_model::vocabulary::OM::resolve("MoveTo");
    IRI image_provider = organization_model::vocabulary::OM::resolve("ImageProvider");
    IRI stereo_image_provider = organization_model::vocabulary::OM::resolve("StereoImageProvider");
    IRI location_image_provider = organization_model::vocabulary::OM::resolve("LocationImageProvider");
    IRI emi_power_provider = organization_model::vocabulary::OM::resolve("EmiPowerProvider");

    owlapi::model::IRIList serviceModels;
    // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#LocationImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider
    serviceModels.push_back(location_image_provider);
    serviceModels.push_back(stereo_image_provider);
    serviceModels.push_back(image_provider);
    serviceModels.push_back(move_to);
    serviceModels.push_back(emi_power_provider);


    {
        owlapi::model::IRIList combinedSystem;

        combinedSystem.push_back(sherpa);
        combinedSystem.push_back(sherpa);
        combinedSystem.push_back(sherpa);

        base::Time startTime = base::Time::now();
        owlapi::model::IRIList supportedModels = ResourceMatch::filterSupportedModels(combinedSystem, serviceModels, ontology);
        base::Time stopTime = base::Time::now();
        BOOST_REQUIRE_MESSAGE(supportedModels.size() == serviceModels.size(), "Services supported by sherpa: computing time: " << (stopTime - startTime).toSeconds());
    }
}

BOOST_AUTO_TEST_CASE(performance_ten_sherpa)
{
    OWLOntology::Ptr ontology = io::OWLOntologyIO::fromFile( getRootDir() + "/test/data/om-schema-v0.6.owl");
    ontology->refresh();

    OWLOntologyTell tell(ontology);
    OWLOntologyAsk ask(ontology);
    tell.initializeDefaultClasses();

    IRI sherpa = organization_model::vocabulary::OM::resolve("Sherpa");
    IRI move_to = organization_model::vocabulary::OM::resolve("MoveTo");
    IRI image_provider = organization_model::vocabulary::OM::resolve("ImageProvider");
    IRI stereo_image_provider = organization_model::vocabulary::OM::resolve("StereoImageProvider");
    IRI location_image_provider = organization_model::vocabulary::OM::resolve("LocationImageProvider");

    owlapi::model::IRIList serviceModels;
    // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#LocationImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
    // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider
    //serviceModels.push_back(location_image_provider);
    //serviceModels.push_back(image_provider);
    //serviceModels.push_back(move_to);
    IRI emi_power_provider = organization_model::vocabulary::OM::resolve("EmiPowerProvider");
    serviceModels.push_back(emi_power_provider);


    {
        owlapi::model::IRIList combinedSystem;

        for(int i=0; i < 10; ++i)
        {
            combinedSystem.push_back(sherpa);
        }

        base::Time startTime = base::Time::now();
        owlapi::model::IRIList supportedModels = ResourceMatch::filterSupportedModels(combinedSystem, serviceModels, ontology);
        base::Time stopTime = base::Time::now();
        BOOST_REQUIRE_MESSAGE(supportedModels.size() == 1, "Services supported by sherpa: computing time: " << (stopTime - startTime).toSeconds());
    }
}

BOOST_AUTO_TEST_CASE(model_bound)
{
    {
        ModelBound::List a;
        a.push_back(ModelBound("m0",1,1));

        ModelBound::List b;
        b.push_back(ModelBound("m0",1,1));

        ModelBound::List list;
        BOOST_REQUIRE_NO_THROW(list = ModelBound::substractMin(a,b));

        ModelBound::List::iterator it = std::find_if(list.begin(), list.end(), [](const ModelBound& b)
                {
                    return b.model == "m0";
                });
        BOOST_REQUIRE_MESSAGE(it != list.end(), "ModelBound m0 exists in list");

        ModelBound mb = *it;
        BOOST_REQUIRE_MESSAGE(mb.min == 0 && mb.max == 0, "Substract to null, but got: " << mb.toString());
    }
    {
        ModelBound::List a;
        a.push_back(ModelBound("m0",5,10));

        ModelBound::List b;
        b.push_back(ModelBound("m0",3,7));

        ModelBound::List list;
        BOOST_REQUIRE_NO_THROW(list = ModelBound::substractMin(a,b));

        ModelBound::List::iterator it = std::find_if(list.begin(), list.end(), [](const ModelBound& b)
                {
                    return b.model == "m0";
                });
        BOOST_REQUIRE_MESSAGE(it != list.end(), "ModelBound m0 exists in list");

        ModelBound mb = *it;
        BOOST_REQUIRE_MESSAGE(mb.min == 2 && mb.max == 7, "Substract to null, but got: " << mb.toString());
    }
    {
        ModelBound::List a;
        a.push_back(ModelBound("m0",5,10));
        a.push_back(ModelBound("m1",1,1));

        ModelBound::List b;
        b.push_back(ModelBound("m0",3,7));

        ModelBound::List list;
        BOOST_REQUIRE_NO_THROW(list = ModelBound::substractMin(a,b));

        ModelBound::List::iterator it = std::find_if(list.begin(), list.end(), [](const ModelBound& b)
                {
                    return b.model == "m0";
                });
        BOOST_REQUIRE_MESSAGE(it != list.end(), "ModelBound m0 exists in list");

        ModelBound mb = *it;
        BOOST_REQUIRE_MESSAGE(mb.min == 2 && mb.max == 7, "SubstractMin expected (2,7), but got: " << mb.toString());

        it = std::find_if(list.begin(), list.end(), [](const ModelBound& b)
                {
                    return b.model == "m1";
                });
        BOOST_REQUIRE_MESSAGE(it != list.end(), "ModelBound m1 exists in list");
    }
    {
        ModelBound::List a;
        a.push_back(ModelBound("m0",5,10));
        a.push_back(ModelBound("m1",1,1));

        ModelBound::List b;
        b.push_back(ModelBound("m0",3,7));

        ModelBound::List list;
        BOOST_REQUIRE_THROW(list = ModelBound::substractMin(b,a), std::invalid_argument);
    }
    {
        ModelBound::List a;
        a.push_back(ModelBound("m0",0,0));

        ModelBound::List b;
        b.push_back(ModelBound("m0",0,2));

        ModelBound::List list;
        BOOST_REQUIRE_NO_THROW(list = ModelBound::substractMin(a,b));
        BOOST_REQUIRE_MESSAGE(list.front().min == 0 && list.front().max == 0, "SubstractMin expected (0,0), but got " << list.front().toString());
    }
    {
        ModelBound::List a;
        a.push_back(ModelBound("m0",0,0));

        ModelBound::List b;
        b.push_back(ModelBound("m0",2,0));

        ModelBound::List list;
        BOOST_REQUIRE_THROW(list = ModelBound::substractMin(a,b), std::invalid_argument);
    }
}

BOOST_AUTO_TEST_CASE(function_mapping)
{
    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-project-transterra.owl"));
    OrganizationModelAsk ask(om);

    IRI mapping = organization_model::vocabulary::OM::resolve("Mapping");
    ModelBound required0(mapping,1,1);
    ModelBound available0(mapping,0,8);

    ModelBound::List required;
    required.push_back(required0);

    ModelBound::List available;
    available.push_back(available0);

    ResourceMatch::Solution solution = ResourceMatch::solve(required, available, ask);
    BOOST_TEST_MESSAGE("Solution found: " << solution.toString());


}


BOOST_AUTO_TEST_SUITE_END()
