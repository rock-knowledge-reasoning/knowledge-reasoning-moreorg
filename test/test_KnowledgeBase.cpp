#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <boost/foreach.hpp>
#include <owl_om/owlapi/KnowledgeBase.hpp>

#include <factpp/Kernel.h>
#include <factpp/Actor.h>

using namespace owlapi;
using namespace owlapi::model;

BOOST_AUTO_TEST_CASE(kb_tell_and_ask)
{
    {
        KnowledgeBase kb;
        kb.setVerbose();

        IRI derived("Derived");
        IRI has("has");

        kb.subclassOf("Base", "Test");
        kb.subclassOf(derived, "Base");
        kb.objectProperty(has);
        kb.functionalProperty(has, KnowledgeBase::OBJECT);
        BOOST_REQUIRE(kb.isFunctionalProperty(has));

        IRI instance("instance");
        kb.instanceOf(instance, derived);

        kb.rangeOf(has, derived, KnowledgeBase::OBJECT);
        kb.domainOf(has, derived, KnowledgeBase::OBJECT);

        reasoner::factpp::ObjectPropertyExpressionList list = kb.getRelatedObjectProperties(derived);
        kb.classify();
        BOOST_CHECK_MESSAGE(!list.empty(), "Experimental evaluation: related properties: " << list.size());
    }
}

BOOST_AUTO_TEST_CASE(kb_create_class_hierarchy)
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
    reasoner::factpp::Axiom a = kb.relatedTo("A","sibling","B", false);

    BOOST_REQUIRE_THROW(kb.refresh(), std::exception);
    kb.retract(a);
    BOOST_REQUIRE_NO_THROW(kb.refresh()) //, "Kb inconsistent if inverses relation both apply" );

    BOOST_REQUIRE_MESSAGE( kb.isRelatedTo("A", "sibling", "B"), "A and B are related");
    BOOST_REQUIRE_THROW( !kb.isRelatedTo("A", "unknown", "B"), std::exception);

    IRIList klasses = kb.allClasses();
    BOOST_REQUIRE_MESSAGE( klasses.size() == 3, "Number of classes is '" << klasses.size() << "' expected 3");

}

BOOST_AUTO_TEST_CASE(kb_handle_om_datavalues)
{
    KnowledgeBase kb;
    kb.subclassOf("Robot","TOP");
    kb.instanceOf("CREX","Robot");
    kb.dataProperty("hasWeight");

    reasoner::factpp::DataValue dataValue = kb.dataValue("100","int");
    kb.valueOf("CREX","hasWeight", dataValue);

    BOOST_REQUIRE_MESSAGE( dataValue.toDouble() == 100.0, "Conversion to double");

    reasoner::factpp::DataValue dataValueString = kb.dataValue("foo","string");
    BOOST_REQUIRE_THROW( dataValueString.toDouble(), std::runtime_error)

    // getNeighbours etc. do not work for data, thus implemented an alternative way 
    // to retrieve information about data value from this representation
    //
    //kb.subclassOf("Sensor","TOP");
    //kb.instanceOf("Camera","Sensor");
    //kb.objectProperty("hasPart");
    //kb.relatedTo("CREX","hasPart", "Camera");
    //const KnowledgeExplorer::TCGNode* node = kb.getReasoningKernel()->buildCompletionTree(klass.get());
    //
    //ClassExpression klass = kb.getClassLazy("Robot");
    //InstanceExpression instance = kb.getInstance("CREX");
    //
    //{
    //    KnowledgeExplorer::TCGNodeVec result;
    //    kb.getReasoningKernel()->getNeighbours ( node, dataRole.get(), result );
    //    BOOST_TEST_MESSAGE("RESULT SIZE: " << result.size());
    //}

    //{
    //    ReasoningKernel::NamesVector vector;
    //    bool data = true;
    //    bool needInverse = false;
    //    kb.getReasoningKernel()->getRelatedRoles (instance.get(), vector, data, needInverse);
    //    BOOST_TEST_MESSAGE("RESULT SIZE: " << vector.size());
    //    // std::vector<const TNamedEntry*>
    //    BOOST_FOREACH(const TNamedEntry* entry, vector)
    //    {
    //        BOOST_TEST_MESSAGE("Entry " << entry->getName() << " id: " << entry->getId());
    //    }
    //}

    //{
    //    ReasoningKernel::TCGRoleSet result;
    //    bool onlyDet = false;
    //    kb.getReasoningKernel()->getDataRoles (node, result, onlyDet );
    //    BOOST_TEST_MESSAGE("ROLE SET SIZE: " << result.size());
    //}
    {
        reasoner::factpp::DataValue dataValue = kb.getDataValue("CREX","hasWeight");
        BOOST_TEST_MESSAGE( "DataValue: " << dataValue.getType() << " - " << dataValue.getValue());
    }
}
