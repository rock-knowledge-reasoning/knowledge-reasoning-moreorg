#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/KnowledgeBase.hpp>
#include <owl_om/OrganizationModel.hpp>

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

    kb.disjoint("A","B", KnowledgeBase::INSTANCE );
    kb.relatedTo("A","sibling","B");
    Axiom a = kb.relatedTo("A","sibling","B", false);

    BOOST_REQUIRE_THROW(kb.refresh(), std::exception);
    kb.retract(a);
    BOOST_REQUIRE_NO_THROW(kb.refresh()) //, "Kb inconsistent if inverses relation both apply" );

    BOOST_REQUIRE_MESSAGE( kb.isRelatedTo("A", "sibling", "B"), "A and B are related");
    BOOST_REQUIRE_THROW( !kb.isRelatedTo("A", "unknown", "B"), std::exception);

}

BOOST_AUTO_TEST_CASE(it_should_handle_om_modelling)
{
    OrganizationModel om;

    KnowledgeBase& kb = om.knowledgeBase();
    kb.setVerbose();
    // General concepts:
    kb.subclassOf("Mission","Thing");
    assert( kb.isSubclassOf("Mission", "Thing") );

    // subclasses includes Mission and BOTTOM
    assert( kb.allSubclassesOf("Thing").size() == 2);

    kb.subclassOf("Resource", "Thing");
    kb.subclassOf("Interface", "Resource");
    kb.subclassOf("Service", "Thing");
    kb.subclassOf("Actor", "Thing");

    kb.subclassOf("Mapping", "Resource");
    kb.subclassOf("Localization", "Resource");
    kb.subclassOf("Locomotion", "Resource");
    kb.subclassOf("MechanicalInterface", "Interface");

    // Define ElectroMechanicalInterface as union of ElectricalInterface and MechanicalInterface
    kb.subclassOf("ElectricalInterface", "Interface");
    ClassExpression emi = kb.intersectionOf("MechanicalInterface", "ElectricalInterface");
    kb.alias("ElectroMechanicalInterface", emi);
    kb.subclassOf("ElectroMechanicalInterface", "Interface");

    kb.subclassOf("EmiActive", "ElectroMechanicalInterface");
    kb.subclassOf("EmiPassive", "ElectroMechanicalInterface");
    kb.subclassOf("EmiNeutral", "ElectroMechanicalInterface");

    // // Define compatible interfaces by using unions that subclass compatiblity
    // kb.subclassOf("Compatibility", "Interface");
    // ClassExpression e = kb.intersectionOf("EmiActive", "EmiPassive");
    // kb.alias("MatchingDevices#0", e);
    // kb.subclassOf("MatchingDevices#0", "Compatibility");

    // BOOST_REQUIRE_MESSAGE( kb.isSubclassOf( kb.intersectionOf("EmiActive", "EmiPassive") , "Compatibility"), "subclass of intersection");

    kb.subclassOf("Camera", "Resource");
    kb.subclassOf("Power", "Resource");

    kb.instanceOf("Mapping/instance#0", "Mapping");
    kb.instanceOf("Mapping/instance#1", "Mapping");
    kb.instanceOf("Mapping/instance#2", "Mapping");

    BOOST_REQUIRE_MESSAGE(kb.isInstanceOf("Mapping/instance#0", "Mapping"), "Instance of Mapping");


    kb.instanceOf("Localization/instance#0", "Localization");
    kb.instanceOf("Localization/instance#1", "Localization");
    kb.instanceOf("Localization/instance#2", "Localization");

    kb.instanceOf("Locomotion/instance#0", "Locomotion");
    kb.instanceOf("Locomotion/instance#1", "Locomotion");
    kb.instanceOf("Locomotion/instance#2", "Locomotion");

    kb.instanceOf("EmiActive/instance#0", "EmiActive");
    kb.instanceOf("EmiActive/instance#1", "EmiActive");
    kb.instanceOf("EmiActive/instance#2", "EmiActive");
    kb.instanceOf("EmiActive/instance#3", "EmiActive");
    kb.instanceOf("EmiActive/instance#11", "EmiActive");

    kb.instanceOf("EmiPassive/instance#0", "EmiPassive");
    kb.instanceOf("EmiPassive/instance#1", "EmiPassive");
    kb.instanceOf("EmiPassive/instance#2", "EmiPassive");
    kb.instanceOf("EmiPassive/instance#3", "EmiPassive");
    kb.instanceOf("EmiPassive/instance#11", "EmiPassive");
    kb.instanceOf("EmiPassive/instance#12", "EmiPassive");
    kb.instanceOf("EmiPassive/instance#13", "EmiPassive");

    kb.instanceOf("Camera/requirement#0", "Camera");
    kb.instanceOf("Camera/requirement#1", "Camera");

    kb.instanceOf("Camera/instance#0", "Camera");
    kb.instanceOf("Camera/instance#1", "Camera");
    kb.instanceOf("Camera/instance#2", "Camera");
    kb.instanceOf("Camera/instance#3", "Camera");

    kb.instanceOf("Power/requirement#0", "Power");

    kb.instanceOf("Power/instance#0", "Power");
    kb.instanceOf("Power/instance#1", "Power");
    kb.instanceOf("Power/instance#2", "Power");
    kb.instanceOf("Power/instance#3", "Power");

    kb.subclassOf("ImageProvider", "Service");
    // to allow a higher arity, e.g., for a stereo camera that requires distinct services
    kb.instanceOf("ImageProvider/requirement#0", "ImageProvider");
    kb.instanceOf("ImageProvider/requirement#1", "ImageProvider");

    kb.subclassOf("StereoImageProvider", "Service");
    kb.subclassOf("StereoImageProvider/requirement#0", "StereoImageProvider");

    kb.refresh();
    BOOST_REQUIRE_MESSAGE( kb.allInstancesOf("Thing").size() != 0, "# of instances of Thing > 0, i.e. " << kb.allInstancesOf("Thing").size());

    // Resource definitions
    kb.transitiveProperty("dependsOn");
    kb.transitiveProperty("provides");
    kb.transitiveProperty("uses");

    //// Service definitions
    kb.subclassOf("MoveTo", "Service");
    kb.instanceOf("MoveTo/requirement#0", "MoveTo");
    kb.relatedTo("MoveTo/requirement#0", "dependsOn", "Mapping/instance#0");
    kb.relatedTo("MoveTo/requirement#0", "dependsOn", "Localization/instance#0");
    kb.relatedTo("MoveTo/requirement#0", "dependsOn", "Locomotion/instance#0");
    kb.relatedTo("MoveTo/requirement#0", "dependsOn", "Power/instance#0");
    kb.alias("move_to", "MoveTo/requirement#0", KnowledgeBase::INSTANCE);
    
    BOOST_REQUIRE_MESSAGE( kb.isRelatedTo("move_to", "dependsOn", "Mapping/instance#0"), "Alias depends on mapping");

    {
        kb.refresh();
        IRIList instanceList = kb.getSameAs("move_to");
        IRI instance = instanceList[0];
        assert(instance == "MoveTo/requirement#0");
        assert ( kb.allRelatedInstances(instance,"dependsOn").size() == 4 );
    }

    kb.relatedTo("ImageProvider/requirement#0", "dependsOn", "Camera/requirement#0");
    kb.relatedTo("ImageProvider/requirement#0", "dependsOn", "Power/requirement#0");

    kb.relatedTo("ImageProvider/requirement#1", "dependsOn", "Camera/requirement#1");
    kb.relatedTo("ImageProvider/requirement#1", "dependsOn", "Power/requirement#1");

    kb.subclassOf("StereoImageProvider", "Service");
    kb.instanceOf("StereoImageProvider/requirement#0", "StereoImageProvider");
    kb.relatedTo("StereoImageProvider/requirement#0", "dependsOn", "ImageProvider/requirement#0");
    kb.relatedTo("StereoImageProvider/requirement#0", "dependsOn", "ImageProvider/requirement#1");

    kb.subclassOf("LocationImageProvider", "Service");
    kb.instanceOf("LocationImageProvider/requirement#0", "LocationImageProvider");
    kb.relatedTo("LocationImageProvider/requirement#0", "dependsOn", "ImageProvider/requirement#0");
    kb.relatedTo("LocationImageProvider/requirement#0", "dependsOn", "MoveTo/requirement#0");

    kb.alias("location_image_provider", "LocationImageProvider/requirement#0", KnowledgeBase::INSTANCE);
    {
        kb.refresh();
        BOOST_REQUIRE ( kb.allRelatedInstances("location_image_provider","dependsOn").size() > 2 );
    }

    kb.subclassOf("Power", "Service");
    kb.subclassOf("EmiPowerProvider", "Power");
    kb.instanceOf("EmiPowerPower/requirement#0", "EmiPowerProvider");
    kb.relatedTo("EmiPowerProvider/requirement#0", "dependsOn", "EmiActive/requirement#0");
    kb.relatedTo("EmiPowerProvider/requirement#0", "dependsOn", "EmiPassive/requirement#0");
    kb.relatedTo("EmiPowerProvider/requirement#0", "dependsOn", "Power/requirement#0");

    //// Actor definition
    kb.subclassOf("Sherpa","Actor");
    kb.instanceOf("Sherpa/instance#0","Sherpa");
    kb.alias("sherpa", "Sherpa/instance#0", KnowledgeBase::INSTANCE);

    kb.subclassOf("CREX","Actor");
    kb.instanceOf("CREX/instance#0","CREX");
    kb.alias("crex", "CREX/instance#0", KnowledgeBase::INSTANCE);

    kb.subclassOf("PayloadCamera","Actor");
    kb.instanceOf("PayloadCamera/instance#0","PayloadCamera");
    kb.alias("payload_camera", "PayloadCamera/instance#0", KnowledgeBase::INSTANCE);

    kb.transitiveProperty("has");
    kb.relatedTo("Sherpa/instance#0", "has", "Mapping/instance#1");
    kb.relatedTo("Sherpa/instance#0", "has", "Localization/instance#1");
    kb.relatedTo("Sherpa/instance#0", "has", "Locomotion/instance#1");
    kb.relatedTo("Sherpa/instance#0", "has", "Camera/instance#1");
    kb.relatedTo("Sherpa/instance#0", "has", "EmiActive/instance#1");
    kb.relatedTo("Sherpa/instance#0", "has", "EmiActive/instance#11");
    kb.relatedTo("Sherpa/instance#0", "has", "EmiPassive/instance#1");
    kb.relatedTo("Sherpa/instance#0", "has", "EmiPassive/instance#11");
    kb.relatedTo("Sherpa/instance#0", "has", "EmiPassive/instance#12");
    kb.relatedTo("Sherpa/instance#0", "has", "EmiPassive/instance#13");
    kb.relatedTo("Sherpa/instance#0", "has", "Power/instance#1");

    kb.relatedTo("CREX/instance#0", "has", "Mapping/instance#2");
    kb.relatedTo("CREX/instance#0", "has", "Localization/instance#2");
    kb.relatedTo("CREX/instance#0", "has", "Locomotion/instance#2");
    kb.relatedTo("CREX/instance#0", "has", "Camera/instance#2");
    kb.relatedTo("CREX/instance#0", "has", "EmiPassive/instance#2");
    kb.relatedTo("CREX/instance#0", "has", "Power/instance#2");

    kb.relatedTo("PayloadCamera/instance#0", "has", "Camera/instance#3");
    kb.relatedTo("PayloadCamera/instance#0", "has", "EmiPassive/instance#3");
    kb.relatedTo("PayloadCamera/instance#0", "has", "EmiActive/instance#3");

    //// Mission requirements
    kb.instanceOf("simple_mission", "Mission");
    kb.relatedTo("simple_mission", "dependsOn", "LocationImageProvider/requirement#0");
    kb.refresh();

    assert ( kb.allRelatedInstances("simple_mission", "dependsOn").size() != 0 );

    kb.allInverseRelatedInstances("Camera/instance#0","dependsOn");
    kb.allInverseRelatedInstances("Camera/instance#1","has");


    {
        ClassExpression emiActiveCompatible = kb.disjunctionOf("EmiPassive","EmiNeutral");
        kb.alias("CompatibleInterfacesEmiActive", emiActiveCompatible);
        // Creates a restriction that all systems that own an EMI are ReconfigurableActors
        ClassExpression forallRestriction = kb.objectPropertyRestriction(restriction::FORALL, "compatibleWith", "CompatibleInterfacesEmiActive");
        kb.alias("EmiActive", forallRestriction);
        kb.refresh();
    }

    {
        ClassExpression emiPassiveCompatible = kb.disjunctionOf("EmiActive","EmiNeutral");
        kb.alias("CompatibleInterfacesEmiPassive", emiPassiveCompatible);
        // Creates a restriction that all systems that own an EMI are ReconfigurableActors
        ClassExpression forallRestriction = kb.objectPropertyRestriction(restriction::FORALL, "compatibleWith", "CompatibleInterfacesEmiPassive");
        kb.alias("EmiPassive", forallRestriction);
        kb.refresh();
    }

    kb.disjoint("CompatibleInterfacesEmiActive", "EmiActive", KnowledgeBase::CLASS);
    BOOST_REQUIRE_NO_THROW( kb.refresh() );
    kb.disjoint("CompatibleInterfacesEmiPassive", "EmiPassive", KnowledgeBase::CLASS);

        BOOST_REQUIRE_NO_THROW( kb.refresh() );
    {
        Axiom a = kb.relatedTo("EmiActive/instance#0", "compatibleWith", "EmiActive/instance#11");
        BOOST_REQUIRE_THROW( kb.refresh(), std::exception);
        kb.retract(a);
        BOOST_REQUIRE_NO_THROW( kb.refresh() );
    }

    {
        Axiom a = kb.relatedTo("EmiPassive/instance#0", "compatibleWith", "EmiPassive/instance#11");
        BOOST_REQUIRE_THROW( kb.refresh(), std::exception);
        kb.retract(a);
        BOOST_REQUIRE_NO_THROW( kb.refresh() );
        BOOST_REQUIRE_MESSAGE(!kb.isRelatedTo("EmiPassive/instance#0", "compatibleWith", "EmiPassive/instance#11"), "Interface is not compatible");
    }

    BOOST_REQUIRE_MESSAGE( om.checkIfCompatible("Sherpa/instance#0","CREX/instance#0"), "Sherpa compatible to CREX");
    BOOST_REQUIRE_MESSAGE( om.checkIfCompatible("Sherpa/instance#0","PayloadCamera/instance#0"), " Sherpa compatible to PayloadCamera" );
    BOOST_REQUIRE_MESSAGE( !om.checkIfCompatible("CREX/instance#0","CREX/instance#0"), "CREX incompatible to CREX");

    om.runInferenceEngine();

    assert( om.checkIfFulfills("Sherpa/instance#0", "LocationImageProvider/requirement#0") );
    assert( kb.isRelatedTo("Sherpa/instance#0", "provides", "LocationImageProvider/requirement#0") );
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

    // Each property that hasInterface with an concept of EmiActive
    // has to be a ReconfigurableActor
    kb.subclassOf("ReconfigurableActor","Actor");
    kb.disjoint("Interface","ReconfigurableActor", KnowledgeBase::CLASS);
    kb.inverseOf("has","availableFor");

    // Creates a restriction that all systems that own an EMI are ReconfigurableActors
    ClassExpression forallRestriction = kb.objectPropertyRestriction(restriction::FORALL, "availableFor", "ReconfigurableActor");
    kb.subclassOf("ElectroMechanicalInterface", forallRestriction);
    kb.refresh();

    //std::vector<std::string> types = kb.typesOf("PayloadCamera/instance#0");
    //for(int i =0; i < types.size(); ++i)
    //{
    //    LOG_WARN_S << types[i];
    //}

    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("PayloadCamera/instance#0", "ReconfigurableActor"), "PayloadCamera instance of ReconfigurableActor" );
    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("Sherpa/instance#0", "ReconfigurableActor"), "Sherpa instance of ReconfigurableActor" );
    BOOST_REQUIRE_MESSAGE( kb.isInstanceOf("CREX/instance#0", "ReconfigurableActor"), "CREX instance of ReconfigurableActor");


    //// A version of Sherpa without interface defined
    kb.instanceOf("Sherpa/instance#1", "Sherpa");
    BOOST_REQUIRE_MESSAGE( !kb.isInstanceOf("Sherpa/instance#1", "ReconfigurableActor"), "Sherpa#1 not instance of ReconfigurableActor");


    // --
    // how to
    // - identify the number of dependencies for a certain instance, i.e.
    // - differentiate between consumables and non-consumables, e.g. such as the interface
    // - count service required
    // - permutations should consider the associated interface (future version)

}

