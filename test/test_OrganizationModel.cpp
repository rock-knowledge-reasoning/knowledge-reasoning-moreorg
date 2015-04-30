#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <organization_model/OrganizationModel.hpp>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/exporter/PDDLExporter.hpp>
#include <numeric/Combinatorics.hpp>
#include <organization_model/metrics/Redundancy.hpp>

#include <owlapi/Vocabulary.hpp>

using namespace organization_model;

BOOST_AUTO_TEST_SUITE(organization_model)

BOOST_AUTO_TEST_CASE(load_file)
{
    OrganizationModel om( getRootDir() + "/test/data/om-schema-v0.5.owl" );
    BOOST_TEST_MESSAGE("Loaded model");
}


BOOST_AUTO_TEST_CASE(function_combination_mapping)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-schema-v0.6.owl"));

    std::map<owlapi::model::IRI, size_t> items;
    items[OM::resolve("Sherpa")] = 3;
    items[OM::resolve("CREX")] = 2;
    items[OM::resolve("Payload")] = 5;
    items[OM::resolve("PayloadCamera")] = 5;

    OrganizationModelAsk ask(om, items);

    Combination2FunctionMap c2f = ask.getCombination2FunctionMap();
    BOOST_TEST_MESSAGE("Combination to function: " << OrganizationModel::toString(c2f));

    Function2CombinationMap f2c = ask.getFunction2CombinationMap();
    BOOST_TEST_MESSAGE("Function to combination: " << OrganizationModel::toString(f2c));
}

BOOST_AUTO_TEST_CASE(resource_support)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getRootDir() + "/test/data/om-schema-v0.6.owl"));

    {
        std::map<owlapi::model::IRI, size_t> items;
        items[OM::resolve("CREX")] = 1;

        OrganizationModelAsk ask(om, items);

        // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider
        // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
        // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider]

        ServiceList services;
        services.push_back( Service(OM::resolve("StereoImageProvider") ) );

        std::vector<ModelCombinationList> combinations = ask.getMinimalResourceSupport(services);

        BOOST_REQUIRE_MESSAGE(combinations.size() == 0, "No combinations that support stereo image provider");
    }
    {
        std::map<owlapi::model::IRI, size_t> items;
        items[OM::resolve("Sherpa")] = 1;
        items[OM::resolve("CREX")] = 1;

        OrganizationModelAsk ask(om, items);

        // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider
        // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
        // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider]

        ServiceList services;
        services.push_back( Service(OM::resolve("StereoImageProvider") ) );

        std::vector<ModelCombinationList> combinations = ask.getMinimalResourceSupport(services);

        BOOST_REQUIRE_MESSAGE(combinations.size() == 2, "Two combinations that support stereo image provider");
    }
}

BOOST_AUTO_TEST_SUITE_END()

//BOOST_AUTO_TEST_CASE(it_should_handle_om_modelling)
//{
//    OrganizationModel om;
//
//    Ontology::Ptr ontology = om.ontology();
//
//    using namespace owlapi::vocabulary;
//
//    // Resource definitions
//    ontology->transitiveProperty(OM::dependsOn());
//    ontology->transitiveProperty(OM::provides());
//    ontology->transitiveProperty(OM::uses());
//    ontology->transitiveProperty(OM::modelledBy());
//    ontology->symmetricProperty(OM::compatibleWith());
//
//    ontology->setVerbose();
//    // General concepts:
//    ontology->subclassOf("Mission","Thing");
//    assert( ontology->isSubclassOf("Mission", "Thing") );
//
//    // subclasses includes Mission and BOTTOM
//    assert( ontology->allSubclassesOf("Thing").size() == 2);
//
//    ontology->subclassOf(OM::Resource(), "Thing");
//    ontology->subclassOf(OM::ResourceModel(), "Thing");
//    ontology->subclassOf(OM::Requirement(), "Thing");
//    ontology->subclassOf(OM::Interface(), OM::Resource());
//    ontology->subclassOf(OM::InterfaceModel(), OM::ResourceModel());
//    ontology->subclassOf(OM::Service(), OM::Resource());
//    ontology->subclassOf(OM::ServiceModel(), OM::ResourceModel());
//    ontology->subclassOf(OM::Actor(), OM::Resource());
//    ontology->subclassOf(OM::ActorModel(), OM::ResourceModel());
//
//    ontology->instanceOf("Mapping", OM::ResourceModel());
//    ontology->instanceOf("Localization", OM::ResourceModel());
//    ontology->instanceOf("Locomotion", OM::ResourceModel());
//
//    ontology->instanceOf("Camera", OM::ResourceModel());
//    ontology->instanceOf("Power", OM::ResourceModel());
//
//    ontology->instanceOf("MechanicalInterface", OM::InterfaceModel());
//    ontology->instanceOf("ElectricalInterface", OM::InterfaceModel());
//    ontology->instanceOf("ElectroMechanicalInterface", OM::InterfaceModel());
//
//    ontology->instanceOf("EmiActive", OM::InterfaceModel());
//    ontology->instanceOf("EmiPassive", OM::InterfaceModel());
//    ontology->instanceOf("EmiNeutral", OM::InterfaceModel());
//
//    ontology->relatedTo("EmiActive", OM::compatibleWith(), "EmiPassive");
//
//    om.createInstance("Mapping/instance#0", OM::Resource(), "Mapping");
//    om.createInstance("Mapping/instance#1", OM::Resource(), "Mapping");
//    om.createInstance("Mapping/instance#2", OM::Resource(), "Mapping");
//    om.createInstance("Mapping/instance#10", OM::Resource(), "Mapping");
//    om.createInstance("Mapping/instance#20", OM::Resource(), "Mapping");
//
//    BOOST_REQUIRE_MESSAGE(ontology->isInstanceOf("Mapping/instance#0", OM::Resource()), "Instance of Mapping");
//    BOOST_REQUIRE_MESSAGE(ontology->isRelatedTo("Mapping/instance#0", OM::modelledBy(), "Mapping"), "Resource Mapping/instance#0 typeOf Mapping");
//
//    om.createInstance("Localization/instance#0", OM::Resource(), "Localization");
//    om.createInstance("Localization/instance#1", OM::Resource(), "Localization");
//    om.createInstance("Localization/instance#10", OM::Resource(), "Localization");
//    om.createInstance("Localization/instance#20", OM::Resource(), "Localization");
//
//    om.createInstance("Locomotion/instance#0", OM::Resource(), "Locomotion");
//    om.createInstance("Locomotion/instance#1", OM::Resource(), "Locomotion");
//    om.createInstance("Locomotion/instance#2", OM::Resource(), "Locomotion");
//    om.createInstance("Locomotion/instance#10", OM::Resource(), "Locomotion");
//    om.createInstance("Locomotion/instance#20", OM::Resource(), "Locomotion");
//
//    om.createInstance("EmiActive/requirement#0", OM::Requirement(), "EmiActive");
//    om.createInstance("EmiActive/instance#0" , OM::Interface(), "EmiActive");
//    om.createInstance("EmiActive/instance#1" , OM::Interface(), "EmiActive");
//    om.createInstance("EmiActive/instance#2" , OM::Interface(), "EmiActive");
//    om.createInstance("EmiActive/instance#3" , OM::Interface(), "EmiActive");
//    om.createInstance("EmiActive/instance#10", OM::Interface(), "EmiActive");
//    om.createInstance("EmiActive/instance#11", OM::Interface(), "EmiActive");
//    om.createInstance("EmiActive/instance#30", OM::Interface(), "EmiActive");
//
//    om.createInstance("EmiPassive/requirement#0", OM::Requirement(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#0" , OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#1" , OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#2" , OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#3" , OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#11", OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#12", OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#13", OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#10", OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#20", OM::Interface(), "EmiPassive");
//    om.createInstance("EmiPassive/instance#30", OM::Interface(), "EmiPassive");
//
//    om.createInstance("Camera/requirement#0" , OM::Requirement(), "Camera");
//    om.createInstance("Camera/requirement#1" , OM::Requirement(), "Camera");
//    om.createInstance("Camera/instance#0" , OM::Resource(), "Camera");
//    om.createInstance("Camera/instance#10" , OM::Resource(), "Camera");
//    om.createInstance("Camera/instance#20" , OM::Resource(), "Camera");
//    om.createInstance("Camera/instance#30" , OM::Resource(), "Camera");
//
//    om.createInstance("Power/requirement#0", OM::Requirement(), "Power");
//    om.createInstance("Power/instance#0", OM::Resource(), "Power");
//
//    ontology->instanceOf("ImageProvider", OM::ServiceModel());
//    // to allow a higher arity, e.g., for a stereo camera that requires distinct services
//    om.createInstance("ImageProvider/requirement#0", OM::Requirement(), "ImageProvider");
//    om.createInstance("ImageProvider/requirement#1", OM::Requirement(), "ImageProvider");
//
//    ontology->instanceOf("StereoImageProvider", OM::ServiceModel());
//    om.createInstance("StereoImageProvider/requirement#0", OM::Service(), "StereoImageProvider");
//
//    ontology->refresh();
//    BOOST_REQUIRE_MESSAGE( ontology->allInstancesOf("Thing").size() != 0, "# of instances of Thing > 0, i.e. " << ontology->allInstancesOf("Thing").size());
//
//    //// Service definitions
//    ontology->instanceOf("MoveTo", OM::ServiceModel());
//    om.createInstance("Mapping/requirement#0", OM::Requirement(), "Mapping");
//    om.createInstance("Localization/requirement#0", OM::Requirement(), "Mapping");
//    om.createInstance("Locomotion/requirement#0", OM::Requirement(), "Mapping");
//    // The following requirement is already defined
//    // om.createInstance("Power/requirement#0", OM::Requirement(), "Power");
//
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "Mapping/requirement#0");
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "Localization/requirement#0");
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "Locomotion/requirement#0");
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "Power/requirement#0");
//
//    BOOST_REQUIRE_MESSAGE( ontology->isRelatedTo("MoveTo", OM::dependsOn(), "Mapping/requirement#0"), "Check dependency");
//
//    om.createInstance("Camera/requirement#0", OM::Requirement(), "Camera");
//    om.createInstance("Power/requirement#0", OM::Requirement(), "Power");
//    ontology->relatedTo("ImageProvider", OM::dependsOn(), "Camera/requirement#0");
//    ontology->relatedTo("ImageProvider", OM::dependsOn(), "Power/requirement#0");
//
//
//    ontology->instanceOf("StereoImageProvider", OM::ServiceModel());
//    ontology->relatedTo("StereoImageProvider", OM::dependsOn(), "ImageProvider/requirement#0");
//    ontology->relatedTo("StereoImageProvider", OM::dependsOn(), "ImageProvider/requirement#1");
//
//    om.createInstance("MoveTo/requirement#0", OM::Requirement(), "MoveTo");
//    ontology->instanceOf("LocationImageProvider", OM::ServiceModel());
//    ontology->relatedTo("LocationImageProvider", OM::dependsOn(), "ImageProvider/requirement#0");
//    ontology->relatedTo("LocationImageProvider", OM::dependsOn(), "MoveTo/requirement#0");
//
//    om.createInstance("LocationImageProvider/requirement#0", OM::Requirement(), "LocationImageProvider");
//    ontology->alias("location_image_provider", "LocationImageProvider/requirement#0", KnowledgeBase::INSTANCE);
//    {
//        ontology->refresh();
//        int allInstances = ontology->allRelatedInstances("location_image_provider",OM::modelledBy()).size();
//        BOOST_REQUIRE_MESSAGE ( ontology->allRelatedInstances("location_image_provider",OM::modelledBy()).size() == 1, "All related instances " << allInstances << " expected > 2" );
//
//        allInstances = ontology->allRelatedInstances("LocationImageProvider",OM::dependsOn()).size();
//        BOOST_REQUIRE_MESSAGE ( ontology->allRelatedInstances("LocationImageProvider",OM::dependsOn()).size() == 2, "All related instances " << allInstances << " expected > 2" );
//    }
//
//    ontology->instanceOf("PowerProvider", OM::ServiceModel());
//    om.createInstance("EmiPowerProvider", OM::Service(), "PowerProvider");
//    ontology->relatedTo("EmiPowerProvider", OM::dependsOn(), "EmiActive/requirement#0");
//    ontology->relatedTo("EmiPowerProvider", OM::dependsOn(), "EmiPassive/requirement#0");
//    ontology->relatedTo("EmiPowerProvider", OM::dependsOn(), "Power/requirement#0");
//    om.createInstance("EmiPowerPower/requirement#0", OM::Requirement(), "EmiPowerProvider");
//
//    //// Actor definition
//    ontology->instanceOf("Sherpa",OM::ActorModel());
//    om.createInstance("Sherpa/instance#0",OM::Actor(), "Sherpa");
//    ontology->alias("sherpa", "Sherpa/instance#0", KnowledgeBase::INSTANCE);
//
//    ontology->instanceOf("CREX",OM::ActorModel());
//    om.createInstance("CREX/instance#0",OM::Actor(), "CREX");
//    ontology->alias("crex", "CREX/instance#0", KnowledgeBase::INSTANCE);
//
//    ontology->instanceOf("PayloadCamera",OM::ActorModel());
//    om.createInstance("PayloadCamera/instance#0",OM::Actor(), "PayloadCamera");
//    ontology->alias("payload_camera", "PayloadCamera/instance#0", KnowledgeBase::INSTANCE);
//
//    ontology->transitiveProperty(OM::has());
//    ontology->relatedTo("Sherpa", OM::has(), "Mapping/instance#10");
//    ontology->relatedTo("Sherpa", OM::has(), "Localization/instance#10");
//    ontology->relatedTo("Sherpa", OM::has(), "Locomotion/instance#10");
//    ontology->relatedTo("Sherpa", OM::has(), "Camera/instance#10");
//    ontology->relatedTo("Sherpa", OM::has(), "EmiActive/instance#10");
//    ontology->relatedTo("Sherpa", OM::has(), "EmiActive/instance#11");
//    ontology->relatedTo("Sherpa", OM::has(), "EmiPassive/instance#10");
//    ontology->relatedTo("Sherpa", OM::has(), "EmiPassive/instance#11");
//    ontology->relatedTo("Sherpa", OM::has(), "EmiPassive/instance#12");
//    ontology->relatedTo("Sherpa", OM::has(), "EmiPassive/instance#13");
//    ontology->relatedTo("Sherpa", OM::has(), "Power/instance#0");
//
//    ontology->relatedTo("CREX", OM::has(), "Mapping/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "Localization/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "Locomotion/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "Camera/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "EmiPassive/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "Power/instance#0");
//
//    ontology->relatedTo("PayloadCamera", OM::has(), "Camera/instance#30");
//    ontology->relatedTo("PayloadCamera", OM::has(), "EmiPassive/instance#30");
//    ontology->relatedTo("PayloadCamera", OM::has(), "EmiActive/instance#30");
//
//    //// Mission requirements
//    ontology->instanceOf("simple_mission", "Mission");
//    ontology->relatedTo("simple_mission", OM::dependsOn(), "LocationImageProvider/requirement#0");
//    ontology->refresh();
//
//    assert ( ontology->allRelatedInstances("simple_mission", OM::dependsOn()).size() != 0 );
//
//    ontology->allInverseRelatedInstances("Camera/instance#0",OM::dependsOn());
//    ontology->allInverseRelatedInstances("Camera/instance#0",OM::has());
//
//    // Export PDDL
//    PDDLExporter exporter;
//    pddl_planner::representation::Domain domain = exporter.toDomain(om);
//
//    BOOST_REQUIRE_MESSAGE(true, "Domain:" << domain.toLISP());
//}

//
//
//BOOST_AUTO_TEST_CASE(it_should_handle_om_modelling_with_punning)
//{
//    using namespace organization_model::vocabulary;
//
//    OrganizationModel om( getRootDir() + "/test/data/om-schema-v0.5.owl" );
//
//    BOOST_TEST_MESSAGE(om.ontology()->toString());
//    {
//        {
//            IRI iri = om.ontology()->relatedInstance(OM::Service(), OM::modelledBy(), OM::ResourceModel());
//            BOOST_REQUIRE_MESSAGE(iri == OM::ServiceModel(), "Service is modelledBy ServiceModel");
//        }
//        {
//            IRI iri = om.ontology()->relatedInstance(OM::ServiceModel(), OM::models(), OM::Resource());
//            BOOST_REQUIRE_MESSAGE(iri == OM::Service(), "ServiceModel is models Service");
//        }
//        {
//            IRI iri = om.getResourceModel(OM::resolve("EmiActive"));
//            BOOST_REQUIRE_MESSAGE(iri == OM::resolve("EmiActive"), "EmiActive should have EmiActive as model");
//        }
//    }
//}
//
//BOOST_AUTO_TEST_CASE(it_should_handle_om_modelling_via_construction)
//{
//    using namespace organization_model::vocabulary;
//    OrganizationModel om( getRootDir() + "/test/data/om-schema-v0.5.owl" );
//
//    {
//        IRI instance = om.createNewInstance(OM::resolve("Sherpa"), true);
//
//        BOOST_TEST_MESSAGE("Created new from model" << instance);
//        BOOST_REQUIRE_MESSAGE( om.ontology()->isInstanceOf(instance, OM::Actor()), "New model instance of Actor");
//    }
//    {
//        IRI instance = om.createNewInstance(OM::resolve("CREX"), true);
//
//        BOOST_TEST_MESSAGE("Created new from model" << instance);
//        BOOST_REQUIRE_MESSAGE( om.ontology()->isInstanceOf(instance, OM::Actor()), "New model instance of Actor");
//    }
//    {
//        IRI instance = om.createNewInstance(OM::resolve("PayloadCamera"), true);
//
//        BOOST_TEST_MESSAGE("Created new from model" << instance);
//        BOOST_REQUIRE_MESSAGE( om.ontology()->isInstanceOf(instance, OM::Actor()), "New model instance of Actor");
//    }
//
//    om.refresh();
//
//    // Export PDDL
//    PDDLExporter exporter;
//    pddl_planner::representation::Domain domain = exporter.toDomain(om);
//    pddl_planner::representation::Problem problem = exporter.toProblem(om);
//
//    std::vector<std::string> distances;
//    distances.push_back("location_p0");
//    distances.push_back("location_c0");
//    distances.push_back("location_p0");
//
//    using namespace organization_model::vocabulary;
//    using namespace pddl_planner::representation;
//
//    std::string locationType = OM::Location().getFragment();
//
//    problem.addObject(TypedItem("location_p0", locationType));
//    problem.addObject(TypedItem("location_c0", locationType));
//    problem.addObject(TypedItem("location_s0", locationType));
//
//    IRIList atomicActors = om.ontology()->allInstancesOf( OM::Actor(), true);
//    BOOST_FOREACH(IRI atomicActor, atomicActors)
//    {
//        std::string instanceName = atomicActor.getFragment();
//        problem.addInitialStatus(Expression("operative",instanceName));
//        problem.addInitialStatus(Expression("at",instanceName, "location_s0"));
//    }
//
//    std::string distance = "10";
//    using namespace numeric;
//    Combination< std::string > combination(distances, 2, EXACT);
//    do
//    {
//        std::vector<std::string> current = combination.current();
//        problem.addInitialStatus( Expression("=", Expression("distance", current[0], current[1]), Expression(distance) ));
//        problem.addInitialStatus( Expression("=", Expression("distance", current[1], current[0]), Expression(distance) ));
//
//    } while(combination.next());
//
//    problem.addInitialStatus( Expression("=", Expression("distance", "location_c0", "location_p0"), Expression("10")));
//
//    BOOST_REQUIRE_MESSAGE(true, "Domain:" << domain.toLISP());
//    BOOST_REQUIRE_MESSAGE(true, "Problem:" << problem.toLISP());
//}
//
//BOOST_AUTO_TEST_CASE(it_should_handle_actorlinkmodel)
//{
//    using namespace organization_model::organization_model;
//
//    EndpointModel model0("http://test#P", "http://test#p0");
//    EndpointModel model1("http://test#P", "http://test#p0");
//
//    BOOST_REQUIRE_MESSAGE( model0 == model1, "Endpoints same");
//    BOOST_REQUIRE_MESSAGE( !(model0 < model1), "Endpoints less than");
//    BOOST_REQUIRE_MESSAGE( !(model1 < model0), "Endpoints less than");
//
//    ActorModelLink link0(model0, model1);
//    ActorModelLink link1(model1, model0);
//
//    BOOST_REQUIRE_MESSAGE( link0 == link1, "ActorModelLinks same");
//    BOOST_REQUIRE_MESSAGE( !(link0 < link1), "ActorModelLinks less than");
//    BOOST_REQUIRE_MESSAGE( !(link1 < link0), "ActorModelLinks less than");
//
//
//    std::vector<ActorModelLink> compositeActor0;
//    std::vector<ActorModelLink> compositeActor1;
//
//    compositeActor0.push_back(link0);
//    std::sort(compositeActor0.begin(), compositeActor0.end());
//    compositeActor1.push_back(link1);
//    std::sort(compositeActor1.begin(), compositeActor1.end());
//
//
//    std::set< std::vector<ActorModelLink> > actorSet;
//    // Extra 
//    {
//        EndpointModel modelA("http://test#PayloadCamera", "http://test#EmiActive-requirement-0");
//        EndpointModel modelB("http://test#PayloadCamera", "http://test#EmiPassive-requirement-0");
//        ActorModelLink link(modelA, modelB);
//        std::vector<ActorModelLink> compositeActor;
//        compositeActor.push_back(link);
//        std::sort(compositeActor.begin(), compositeActor.end());
//        actorSet.insert(compositeActor);
//    }
//    // Extra 
//    {
//        EndpointModel modelA("http://test#PayloadCamera", "http://test#EmiPassive-requirement-0");
//        EndpointModel modelB("http://test#Sherpa", "http://test#EmiActive-requirement-0");
//        ActorModelLink link(modelA, modelB);
//        std::vector<ActorModelLink> compositeActor;
//        compositeActor.push_back(link);
//        std::sort(compositeActor.begin(), compositeActor.end());
//        actorSet.insert(compositeActor);
//    }
//    // Extra 
//    {
//        EndpointModel modelA("http://test#PayloadCamera", "http://test#EmiPassive-requirement-0");
//        EndpointModel modelB("http://test#Sherpa", "http://test#EmiActive-requirement-1");
//        ActorModelLink link(modelA, modelB);
//        std::vector<ActorModelLink> compositeActor;
//        compositeActor.push_back(link);
//        std::sort(compositeActor.begin(), compositeActor.end());
//        actorSet.insert(compositeActor);
//    }
//    // Extra 
//    {
//        EndpointModel modelA("http://test#PayloadCamera", "http://test#EmiActive-requirement-0");
//        EndpointModel modelB("http://test#PayloadCamera", "http://test#EmiPassive-requirement-0");
//        ActorModelLink link(modelA, modelB);
//        std::vector<ActorModelLink> compositeActor;
//        compositeActor.push_back(link);
//        std::sort(compositeActor.begin(), compositeActor.end());
//        actorSet.insert(compositeActor);
//    }
//    //BOOST_TEST_MESSAGE("ActorSet\n" << actorSet);
//
//}
