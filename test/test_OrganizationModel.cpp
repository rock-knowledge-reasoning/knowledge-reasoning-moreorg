#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owlapi/io/OWLOntologyIO.hpp>
#include <moreorg/OrganizationModel.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/exporter/PDDLExporter.hpp>
#include <numeric/Combinatorics.hpp>
#include <moreorg/metrics/Redundancy.hpp>
#include <moreorg/vocabularies/OM.hpp>

using namespace moreorg;
using namespace moreorg::vocabulary;

BOOST_AUTO_TEST_SUITE(moreorg)

BOOST_AUTO_TEST_CASE(load_file)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());

    OrganizationModelAsk ask(om, ModelPool(), false);
    owlapi::model::OWLOntologyAsk ontologyAsk(om->ontology());
    BOOST_TEST_MESSAGE("Loaded model" << ontologyAsk.allClasses());
}


BOOST_AUTO_TEST_CASE(function_combination_mapping)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()) );

    ModelPool items;
    items[OM::resolve("BaseCamp")] = 1;

    OrganizationModelAsk ask(om, items, true);
    {
        algebra::SupportType supportType = ask.getSupportType(OM::resolve("PayloadLogisticHub"), items);
        BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support for payload logistic hub");
    }
    {
        algebra::SupportType supportType = ask.getSupportType(OM::resolve("TransportProvider"), items);
        BOOST_REQUIRE_MESSAGE(supportType == algebra::PARTIAL_SUPPORT, "Partial support for transport service expected, but is : " << algebra::SupportTypeTxt[supportType]);
    }

    items[OM::resolve("Sherpa")] = 3;
    items[OM::resolve("CREX")] = 3;
    items[OM::resolve("CoyoteIII")] = 3;

    ModelPool::Set modelPoolSet = items.allCombinations();
    BOOST_TEST_MESSAGE("All combinations of three agent types with cardinality 3: " << ModelPool::toString(modelPoolSet) );

    items[OM::resolve("Payload")] = 2;
    items[OM::resolve("PayloadCamera")] = 5;
    items[OM::resolve("PayloadBattery")] = 5;
    items[OM::resolve("PayloadSoilSampler")] = 1;
    items[OM::resolve("BaseCamp")] = 3;

    FunctionalityMapping fm = ask.computeFunctionalityMapping(items, true);
    BOOST_TEST_MESSAGE("FunctionalityMapping " << fm.toString());

    fm.save("/tmp/test-moreorg-functionality-mapping");
    FunctionalityMapping fmLoaded = FunctionalityMapping::fromFile("/tmp/test-moreorg-functionality-mapping");

    BOOST_REQUIRE_MESSAGE(fmLoaded.getModelPool() == items, "Loaded model has all resources" << fmLoaded.toString());
    BOOST_REQUIRE_MESSAGE(fmLoaded.getFunctionalSaturationBound().at(OM::resolve("BaseCamp"))
            == 1, "Saturation bound loaded" << fmLoaded.toString());
}

BOOST_AUTO_TEST_CASE(resource_support)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    {
        ModelPool items;
        items[OM::resolve("CREX")] = 1;

        OrganizationModelAsk ask(om, items);

        // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider
        // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
        // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider]

        Resource::Set functionalities;
        functionalities.insert( Resource(OM::resolve("StereoImageProvider")) );

        ModelPool::Set modelPools = ask.getResourceSupport(functionalities);
        BOOST_REQUIRE_MESSAGE(modelPools.size() == 0, "No combinations that support stereo image provider expected 0 was " << modelPools.size());
    }
    {
        ModelPool items;
        items[OM::resolve("Sherpa")] = 1;
        items[OM::resolve("CREX")] = 1;

        OrganizationModelAsk ask(om, items, false);

        Resource::Set functionalities;
        functionalities.insert( Resource(OM::resolve("StereoImageProvider") ) );

        ModelPool::Set combinations = ask.getResourceSupport(functionalities);
        BOOST_REQUIRE_MESSAGE(combinations.size() == 2, "Two combinations of "
                << items.toString(4)
                << " that support stereo image provider, but were " << combinations.size()
                << " "
                << ModelPool::toString(combinations) );
    }
    {
        ModelPool items;
        items[OM::resolve("PayloadBattery")] = 2;
        items[OM::resolve("PayloadCamera")] = 2;

        {
            OrganizationModelAsk ask(om, items, false);

            Resource::Set functionalities;
            functionalities.insert( Resource(OM::resolve("StereoImageProvider") ) );

            ModelPool::Set combinations = ask.getResourceSupport(functionalities);
            BOOST_REQUIRE_MESSAGE(combinations.size() == 2, "Two combinations "
                    << items.toString(4)
                    << " that support stereo image provider, but were " << combinations.size()
                    << " "
                    << ModelPool::toString(combinations) );
        }
        {
            OrganizationModelAsk ask(om, items, true);

            Resource::Set functionalities;
            functionalities.insert( Resource(OM::resolve("StereoImageProvider") ) );

            ModelPool::Set combinations = ask.getResourceSupport(functionalities);
            BOOST_REQUIRE_MESSAGE(combinations.size() == 1, "With functional bound only one combination that supports stereo image provider, but were " << combinations.size()
                    << " "
                    << ModelPool::toString(combinations) );
        }
    }

    {
        ModelPool items;
        items[OM::resolve("Sherpa")] = 3;
        items[OM::resolve("CREX")] = 2;
        items[OM::resolve("Payload")] = 10;

        OrganizationModelAsk ask(om, items);

        // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider
        // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
        // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider]

        Resource::Set functionalities;
        functionalities.insert( Resource(OM::resolve("StereoImageProvider") ) );
        functionalities.insert( Resource(OM::resolve("ImageProvider") ) );
        functionalities.insert( Resource(OM::resolve("EmiPowerProvider") ) );

        //std::set<ModelCombinationSet> combinations = ask.getResourceSupport(services);
        //std::set<ModelCombinationSet>::const_iterator cit = combinations.begin();
        //for(; cit != combinations.end(); ++cit)
        //{
        //    BOOST_TEST_MESSAGE("ModelCombination");
        //    const ModelCombinationSet& list = *cit;
        //    ModelCombinationSet::const_iterator mit = list.begin();
        //    for(; mit != list.end(); ++mit)
        //    {
        //        BOOST_TEST_MESSAGE("    " << IRI::toString(*mit, true));
        //    }
        //}

        //BOOST_REQUIRE_MESSAGE(combinations.size() > 1000, "Combinations that support stereo image provider, was " << combinations.size());

        {
            OrganizationModelAsk ask(om);
            BOOST_REQUIRE_THROW(ask.getFunctionalSaturationBound(functionalities), std::invalid_argument);
        }
        {
            OrganizationModelAsk ask(om, items);
            ModelPool modelPoolBounded = ask.getFunctionalSaturationBound(functionalities);
            BOOST_TEST_MESSAGE("ModelPoolBounded: " << modelPoolBounded.toString());

            {
                OrganizationModelAsk minimalAsk(om, modelPoolBounded, false);
                ModelPool::Set combinations = minimalAsk.getResourceSupport(functionalities);
                std::set<ModelPool>::const_iterator cit = combinations.begin();
                for(; cit != combinations.end(); ++cit)
                {
                    BOOST_TEST_MESSAGE("ModelCombination: " << cit->toString() );
                }


                BOOST_CHECK_MESSAGE(combinations.size() == 6,
                        "Without functional saturation bound: six combinations that support stereo image provider, image provider and emi power provider, was " << combinations.size() << " "
                        << ModelPool::toString(combinations, 4) << " "
                        << "\nBounded model pool: " << ModelPoolDelta(modelPoolBounded).toString());
            }
            {
                OrganizationModelAsk minimalAsk(om, modelPoolBounded, true);
                ModelPool::Set combinations = minimalAsk.getBoundedResourceSupport(functionalities);
                std::set<ModelPool>::const_iterator cit = combinations.begin();
                for(; cit != combinations.end(); ++cit)
                {
                    BOOST_TEST_MESSAGE("ModelCombination: " << cit->toString() );
                }

                BOOST_CHECK_MESSAGE(combinations.size() == 1, "With functional saturation bound: one combinations that support stereo image provider, image provider and emi power provider, was " << combinations.size() << " "
                        << ModelPool::toString(combinations, 4) << " "
                        << "\nBounded model pool: " << ModelPoolDelta(modelPoolBounded).toString()
                        << "\nFunctionality: " << minimalAsk.getFunctionalityMapping().toString());
            }
        }
    }
}

BOOST_AUTO_TEST_CASE(resource_support_crex)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());

    {
        ModelPool items;
        items[OM::resolve("CREX")] = 1;

        OrganizationModelAsk ask(om, items);

        // http://www.rock-robotics.org/2014/01/om-schema#StereoImageProvider
        // http://www.rock-robotics.org/2014/01/om-schema#ImageProvider,
        // http://www.rock-robotics.org/2014/01/om-schema#EmiPowerProvider]

        Resource::Set functionalities;
        functionalities.insert( Resource(OM::resolve("StereoImageProvider") ) );

        ModelPool::Set combinations = ask.getResourceSupport(functionalities);

        BOOST_REQUIRE_MESSAGE(combinations.size() == 0, "No combinations that support stereo image provider expected 0 was " << combinations.size());
    }
}

BOOST_AUTO_TEST_CASE(load_qudt)
{
    using namespace owlapi::model;
    using namespace owlapi::io;

    OWLOntology::Ptr qudt_ontology =
        OWLOntologyIO::load(IRI("http://qudt.org/2.1/schema/qudt"));
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
//    ontology->instanceOf("PowerSource", OM::ResourceModel());
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
//    om.createInstance("PowerSource/requirement#0", OM::Requirement(), "PowerSource");
//    om.createInstance("PowerSource/instance#0", OM::Resource(), "PowerSource");
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
//    // om.createInstance("PowerSource/requirement#0", OM::Requirement(), "PowerSource");
//
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "Mapping/requirement#0");
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "Localization/requirement#0");
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "Locomotion/requirement#0");
//    ontology->relatedTo("MoveTo", OM::dependsOn(), "PowerSource/requirement#0");
//
//    BOOST_REQUIRE_MESSAGE( ontology->isRelatedTo("MoveTo", OM::dependsOn(), "Mapping/requirement#0"), "Check dependency");
//
//    om.createInstance("Camera/requirement#0", OM::Requirement(), "Camera");
//    om.createInstance("PowerSource/requirement#0", OM::Requirement(), "PowerSource");
//    ontology->relatedTo("ImageProvider", OM::dependsOn(), "Camera/requirement#0");
//    ontology->relatedTo("ImageProvider", OM::dependsOn(), "PowerSource/requirement#0");
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
//    ontology->relatedTo("EmiPowerProvider", OM::dependsOn(), "PowerSource/requirement#0");
//    om.createInstance("EmiPowerPowerSource/requirement#0", OM::Requirement(), "EmiPowerProvider");
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
//    ontology->relatedTo("Sherpa", OM::has(), "PowerSource/instance#0");
//
//    ontology->relatedTo("CREX", OM::has(), "Mapping/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "Localization/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "Locomotion/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "Camera/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "EmiPassive/instance#20");
//    ontology->relatedTo("CREX", OM::has(), "PowerSource/instance#0");
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
//    using namespace moreorg::vocabulary;
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
//    using namespace moreorg::vocabulary;
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
//    using namespace moreorg::vocabulary;
//    using namespace pddl_planner::representation;
//
//    std::string locationType = OM::Location().getFragment();
//
//    problem.addObject(TypedItem("location_p0", locationType));
//    problem.addObject(TypedItem("location_c0", locationType));
//    problem.addObject(TypedItem("location_s0", locationType));
//
//    IRIList atomicActors = om.ontology()->allInstancesOf( OM::Actor(), true);
//    for(IRI atomicActor : atomicActors)
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
//    using namespace moreorg::organization_model;
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
