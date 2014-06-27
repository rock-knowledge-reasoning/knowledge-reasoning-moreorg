#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/KnowledgeBase.hpp>
#include <owl_om/OrganizationModel.hpp>

#include <owl_om/exporter/PDDLExporter.hpp>

#include <owl_om/db/Ontology.hpp>
using namespace owl_om;

BOOST_AUTO_TEST_CASE(it_should_create_class_hierarchy)
{
    KnowledgeBase kb;
    kb.setVerbose();
    kb.subclassOf("Derived", "Base");
    kb.subclassOf("DerivedDerived", "Derived");
    BOOST_REQUIRE_MESSAGE( kb.isSubclassOf("Derived", "Base"), "Derived is subclass of base");
    BOOST_REQUIRE_MESSAGE( kb.isSubclassOf("DerivedDerived", "Base"), "DerivedDerived is subclass of base");
    kb.instanceOf("Instance", "DerivedDerived");
    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("Instance", "DerivedDerived"), "instance of DerivedDerived");
    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("Instance", "Base"), "instance of Base");

    kb.instanceOf("A","Base");
    kb.instanceOf("B","Base");

    IRIList directInstances = kb.allInstancesOf("Base", true);
    BOOST_REQUIRE_MESSAGE(directInstances.size() == 2, "All direct instances of BASE: '" << directInstances.size() << "' expected 2");

    kb.disjoint("A","B", KnowledgeBase::INSTANCE );
    kb.relatedTo("A","sibling","B");
    Axiom a = kb.relatedTo("A","sibling","B", false);

    BOOST_REQUIRE_THROW(kb.refresh(), std::exception);
    kb.retract(a);
    BOOST_REQUIRE_NO_THROW(kb.refresh()) //, "Kb inconsistent if inverses relation both apply" );

    BOOST_REQUIRE_MESSAGE( kb.isRelatedTo("A", "sibling", "B"), "A and B are related");
    BOOST_REQUIRE_THROW( !kb.isRelatedTo("A", "unknown", "B"), std::exception);

    IRIList klasses = kb.allClasses();
    BOOST_REQUIRE_MESSAGE( klasses.size() == 3, "Number of classes is '" << klasses.size() << "' expected 3");

}

BOOST_AUTO_TEST_CASE(it_should_handle_om_modelling)
{
    OrganizationModel om;

    KnowledgeBase& kb = om.knowledgeBase();

    // Resource definitions
    kb.transitiveProperty("dependsOn");
    kb.transitiveProperty("provides");
    kb.transitiveProperty("uses");
    kb.transitiveProperty("modeledBy");
    kb.symmetricProperty("compatibleWith");

    kb.setVerbose();
    // General concepts:
    kb.subclassOf("Mission","Thing");
    assert( kb.isSubclassOf("Mission", "Thing") );

    // subclasses includes Mission and BOTTOM
    assert( kb.allSubclassesOf("Thing").size() == 2);

    kb.subclassOf("Resource", "Thing");
    kb.subclassOf("ResourceModel", "Thing");
    kb.subclassOf("ResourceRequirement", "Thing");
    kb.subclassOf("Interface", "Resource");
    kb.subclassOf("InterfaceModel", "ResourceModel");
    kb.subclassOf("Service", "Resource");
    kb.subclassOf("ServiceModel", "ResourceModel");
    kb.subclassOf("Actor", "Resource");
    kb.subclassOf("ActorModel", "ResourceModel");

    kb.instanceOf("Mapping", "ResourceModel");
    kb.instanceOf("Localization", "ResourceModel");
    kb.instanceOf("Locomotion", "ResourceModel");

    kb.instanceOf("Camera", "ResourceModel");
    kb.instanceOf("Power", "ResourceModel");

    kb.instanceOf("MechanicalInterface", "InterfaceModel");
    kb.instanceOf("ElectricalInterface", "InterfaceModel");
    kb.instanceOf("ElectroMechanicalInterface", "InterfaceModel");

    kb.instanceOf("EmiActive", "InterfaceModel");
    kb.instanceOf("EmiPassive", "InterfaceModel");
    kb.instanceOf("EmiNeutral", "InterfaceModel");

    kb.relatedTo("EmiActive", "compatibleWith", "EmiPassive");

    om.createInstance("Mapping/instance#0", "Resource", "Mapping");
    om.createInstance("Mapping/instance#1", "Resource", "Mapping");
    om.createInstance("Mapping/instance#2", "Resource", "Mapping");
    om.createInstance("Mapping/instance#10", "Resource", "Mapping");
    om.createInstance("Mapping/instance#20", "Resource", "Mapping");

    BOOST_REQUIRE_MESSAGE(kb.isInstanceOf("Mapping/instance#0", "Resource"), "Instance of Mapping");
    BOOST_REQUIRE_MESSAGE(kb.isRelatedTo("Mapping/instance#0", "modeledBy", "Mapping"), "Resource Mapping/instance#0 typeOf Mapping");

    om.createInstance("Localization/instance#0", "Resource", "Localization");
    om.createInstance("Localization/instance#1", "Resource", "Localization");
    om.createInstance("Localization/instance#10", "Resource", "Localization");
    om.createInstance("Localization/instance#20", "Resource", "Localization");

    om.createInstance("Locomotion/instance#0", "Resource", "Locomotion");
    om.createInstance("Locomotion/instance#1", "Resource", "Locomotion");
    om.createInstance("Locomotion/instance#2", "Resource", "Locomotion");
    om.createInstance("Locomotion/instance#10", "Resource", "Locomotion");
    om.createInstance("Locomotion/instance#20", "Resource", "Locomotion");

    om.createInstance("EmiActive/requirement#0", "ResourceRequirement", "EmiActive");
    om.createInstance("EmiActive/instance#0" , "Interface", "EmiActive");
    om.createInstance("EmiActive/instance#1" , "Interface", "EmiActive");
    om.createInstance("EmiActive/instance#2" , "Interface", "EmiActive");
    om.createInstance("EmiActive/instance#3" , "Interface", "EmiActive");
    om.createInstance("EmiActive/instance#10", "Interface", "EmiActive");
    om.createInstance("EmiActive/instance#11", "Interface", "EmiActive");
    om.createInstance("EmiActive/instance#30", "Interface", "EmiActive");

    om.createInstance("EmiPassive/requirement#0", "ResourceRequirement", "EmiPassive");
    om.createInstance("EmiPassive/instance#0" , "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#1" , "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#2" , "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#3" , "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#11", "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#12", "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#13", "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#10", "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#20", "Interface", "EmiPassive");
    om.createInstance("EmiPassive/instance#30", "Interface", "EmiPassive");

    om.createInstance("Camera/requirement#0" , "ResourceRequirement", "Camera");
    om.createInstance("Camera/requirement#1" , "ResourceRequirement", "Camera");
    om.createInstance("Camera/instance#0" , "Resource", "Camera");
    om.createInstance("Camera/instance#10" , "Resource", "Camera");
    om.createInstance("Camera/instance#20" , "Resource", "Camera");
    om.createInstance("Camera/instance#30" , "Resource", "Camera");

    om.createInstance("Power/requirement#0", "ResourceRequirement", "Power");
    om.createInstance("Power/instance#0", "Resource", "Power");

    kb.instanceOf("ImageProvider", "ServiceModel");
    // to allow a higher arity, e.g., for a stereo camera that requires distinct services
    om.createInstance("ImageProvider/requirement#0", "ResourceRequirement", "ImageProvider");
    om.createInstance("ImageProvider/requirement#1", "ResourceRequirement", "ImageProvider");

    kb.instanceOf("StereoImageProvider", "ServiceModel");
    om.createInstance("StereoImageProvider/requirement#0", "Service", "StereoImageProvider");

    kb.refresh();
    BOOST_REQUIRE_MESSAGE( kb.allInstancesOf("Thing").size() != 0, "# of instances of Thing > 0, i.e. " << kb.allInstancesOf("Thing").size());

    //// Service definitions
    kb.instanceOf("MoveTo", "ServiceModel");
    om.createInstance("Mapping/requirement#0", "ResourceRequirement", "Mapping");
    om.createInstance("Localization/requirement#0", "ResourceRequirement", "Mapping");
    om.createInstance("Locomotion/requirement#0", "ResourceRequirement", "Mapping");
    // The following requirement is already defined
    // om.createInstance("Power/requirement#0", "ResourceRequirement", "Power");

    kb.relatedTo("MoveTo", "dependsOn", "Mapping/requirement#0");
    kb.relatedTo("MoveTo", "dependsOn", "Localization/requirement#0");
    kb.relatedTo("MoveTo", "dependsOn", "Locomotion/requirement#0");
    kb.relatedTo("MoveTo", "dependsOn", "Power/requirement#0");
    
    BOOST_REQUIRE_MESSAGE( kb.isRelatedTo("MoveTo", "dependsOn", "Mapping/requirement#0"), "Check dependency");

    om.createInstance("Camera/requirement#0", "ResourceRequirement", "Camera");
    om.createInstance("Power/requirement#0", "ResourceRequirement", "Power");
    kb.relatedTo("ImageProvider", "dependsOn", "Camera/requirement#0");
    kb.relatedTo("ImageProvider", "dependsOn", "Power/requirement#0");


    kb.instanceOf("StereoImageProvider", "ServiceModel");
    kb.relatedTo("StereoImageProvider", "dependsOn", "ImageProvider/requirement#0");
    kb.relatedTo("StereoImageProvider", "dependsOn", "ImageProvider/requirement#1");

    om.createInstance("MoveTo/requirement#0", "ResourceRequirement", "MoveTo");
    kb.instanceOf("LocationImageProvider", "ServiceModel");
    kb.relatedTo("LocationImageProvider", "dependsOn", "ImageProvider/requirement#0");
    kb.relatedTo("LocationImageProvider", "dependsOn", "MoveTo/requirement#0");

    om.createInstance("LocationImageProvider/requirement#0", "ResourceRequirement", "LocationImageProvider");
    kb.alias("location_image_provider", "LocationImageProvider/requirement#0", KnowledgeBase::INSTANCE);
    {
        kb.refresh();
        int allInstances = kb.allRelatedInstances("location_image_provider","modeledBy").size();
        BOOST_REQUIRE_MESSAGE ( kb.allRelatedInstances("location_image_provider","modeledBy").size() == 1, "All related instances " << allInstances << " expected > 2" );

        allInstances = kb.allRelatedInstances("LocationImageProvider","dependsOn").size();
        BOOST_REQUIRE_MESSAGE ( kb.allRelatedInstances("LocationImageProvider","dependsOn").size() == 2, "All related instances " << allInstances << " expected > 2" );
    }

    kb.instanceOf("PowerProvider", "ServiceModel");
    om.createInstance("EmiPowerProvider", "Service", "PowerProvider");
    kb.relatedTo("EmiPowerProvider", "dependsOn", "EmiActive/requirement#0");
    kb.relatedTo("EmiPowerProvider", "dependsOn", "EmiPassive/requirement#0");
    kb.relatedTo("EmiPowerProvider", "dependsOn", "Power/requirement#0");
    om.createInstance("EmiPowerPower/requirement#0", "ResourceRequirement", "EmiPowerProvider");

    //// Actor definition
    kb.instanceOf("Sherpa","ActorModel");
    om.createInstance("Sherpa/instance#0","Actor", "Sherpa");
    kb.alias("sherpa", "Sherpa/instance#0", KnowledgeBase::INSTANCE);

    kb.instanceOf("CREX","ActorModel");
    om.createInstance("CREX/instance#0","Actor", "CREX");
    kb.alias("crex", "CREX/instance#0", KnowledgeBase::INSTANCE);

    kb.instanceOf("PayloadCamera","ActorModel");
    om.createInstance("PayloadCamera/instance#0","Actor", "PayloadCamera");
    kb.alias("payload_camera", "PayloadCamera/instance#0", KnowledgeBase::INSTANCE);

    kb.transitiveProperty("has");
    kb.relatedTo("Sherpa", "has", "Mapping/instance#10");
    kb.relatedTo("Sherpa", "has", "Localization/instance#10");
    kb.relatedTo("Sherpa", "has", "Locomotion/instance#10");
    kb.relatedTo("Sherpa", "has", "Camera/instance#10");
    kb.relatedTo("Sherpa", "has", "EmiActive/instance#10");
    kb.relatedTo("Sherpa", "has", "EmiActive/instance#11");
    kb.relatedTo("Sherpa", "has", "EmiPassive/instance#10");
    kb.relatedTo("Sherpa", "has", "EmiPassive/instance#11");
    kb.relatedTo("Sherpa", "has", "EmiPassive/instance#12");
    kb.relatedTo("Sherpa", "has", "EmiPassive/instance#13");
    kb.relatedTo("Sherpa", "has", "Power/instance#0");

    kb.relatedTo("CREX", "has", "Mapping/instance#20");
    kb.relatedTo("CREX", "has", "Localization/instance#20");
    kb.relatedTo("CREX", "has", "Locomotion/instance#20");
    kb.relatedTo("CREX", "has", "Camera/instance#20");
    kb.relatedTo("CREX", "has", "EmiPassive/instance#20");
    kb.relatedTo("CREX", "has", "Power/instance#0");

    kb.relatedTo("PayloadCamera", "has", "Camera/instance#30");
    kb.relatedTo("PayloadCamera", "has", "EmiPassive/instance#30");
    kb.relatedTo("PayloadCamera", "has", "EmiActive/instance#30");

    //// Mission requirements
    kb.instanceOf("simple_mission", "Mission");
    kb.relatedTo("simple_mission", "dependsOn", "LocationImageProvider/requirement#0");
    kb.refresh();

    assert ( kb.allRelatedInstances("simple_mission", "dependsOn").size() != 0 );

    kb.allInverseRelatedInstances("Camera/instance#0","dependsOn");
    kb.allInverseRelatedInstances("Camera/instance#0","has");

    BOOST_REQUIRE_MESSAGE( om.checkIfCompatible("Sherpa/instance#0","CREX/instance#0"), "Sherpa compatible to CREX");
    BOOST_REQUIRE_MESSAGE( om.checkIfCompatible("Sherpa/instance#0","PayloadCamera/instance#0"), " Sherpa compatible to PayloadCamera" );
    BOOST_REQUIRE_MESSAGE( !om.checkIfCompatible("CREX/instance#0","CREX/instance#0"), "CREX incompatible to CREX");

    om.runInferenceEngine();

    BOOST_REQUIRE( om.checkIfFulfills("Sherpa/instance#0", "LocationImageProvider/requirement#0") );
    BOOST_REQUIRE( kb.isRelatedTo("Sherpa", "provides", "LocationImageProvider") );
    {
        std::vector<std::string> newActors = om.computeActorsFromRecombination();
        std::vector<std::string>::iterator ait = newActors.begin();
        for(; ait != newActors.end(); ++ait)
        {
            LOG_INFO_S << "New actor: '" << *ait;
        }
        om.runInferenceEngine();
    }
    {
        std::vector<std::string> newActors = om.computeActorsFromRecombination();
        std::vector<std::string>::iterator ait = newActors.begin();
        for(; ait != newActors.end(); ++ait)
        {
            LOG_INFO_S << "New actor after inference: '" << *ait;
        }
    }

//    // Each property that hasInterface with an concept of EmiActive
//    // has to be a ReconfigurableActor
//    kb.subclassOf("ReconfigurableActor","Actor");
//    kb.disjoint("Interface","ReconfigurableActor", KnowledgeBase::CLASS);
//    kb.inverseOf("has","availableFor");
//
//    // Creates a restriction that all systems that own an EMI are ReconfigurableActors
//    ClassExpression forallRestriction = kb.objectPropertyRestriction(restriction::FORALL, "availableFor", "ReconfigurableActor");
//    kb.subclassOf("ElectroMechanicalInterface", forallRestriction);
//    kb.refresh();
//
//    //std::vector<std::string> types = kb.typesOf("PayloadCamera/instance#0");
//    //for(int i =0; i < types.size(); ++i)
//    //{
//    //    LOG_WARN_S << types[i];
//    //}
//
//    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("PayloadCamera/instance#0", "ReconfigurableActor"), "PayloadCamera instance of ReconfigurableActor" );
//    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("Sherpa/instance#0", "ReconfigurableActor"), "Sherpa instance of ReconfigurableActor" );
//    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("CREX/instance#0", "ReconfigurableActor"), "CREX instance of ReconfigurableActor");
//
//
//    //// A version of Sherpa without interface defined
//    kb.instanceOf("Sherpa/instance#1", "Sherpa");
//    BOOST_REQUIRE_MESSAGE( !kb.isInstanceOf("Sherpa/instance#1", "ReconfigurableActor"), "Sherpa#1 not instance of ReconfigurableActor");
//
//
//    // --
//    // how to
//    // - identify the number of dependencies for a certain instance, i.e.
//    // - differentiate between consumables and non-consumables, e.g. such as the interface
//    // - count service required
//    // - permutations should consider the associated interface (future version)
//    //
//
    // Export PDDL
    PDDLExporter exporter;
    pddl_planner::representation::Domain domain = exporter.toDomain(om);

    BOOST_REQUIRE_MESSAGE(true, "Domain:" << domain.toLISP());
}

BOOST_AUTO_TEST_CASE(it_should_handle_om_modelling_from_owl)
{
    db::Ontology::Ptr ontology = db::Ontology::fromFile( getRootDir() + "/test/data/om-schema-v0.2.owl" );
    ontology->refresh();
    owl_om::vocabulary::Custom rock("http://www.rock-robotics.org/2014/01/om-schema#");

    BOOST_REQUIRE_MESSAGE("http://www.rock-robotics.org/2014/01/om-schema#compatibleWith" == rock["compatibleWith"], "ObjectProperty: compatible with is valid");
    BOOST_REQUIRE_NO_THROW(ontology->getObjectProperty( rock["compatibleWith"] ) );
    BOOST_REQUIRE_MESSAGE(ontology->isRelatedTo( rock["EmiActive"], rock["compatibleWith"], rock["EmiPassive"]), "Ontology: EmiActive compatibleWith EmiPassive");
}
