#ifndef OWL_OM_KNOWLEDGE_BASE_HPP
#define OWL_OM_KNOWLEDGE_BASE_HPP

#include <string.h>
#include <map>
#include <vector>
#include <stdexcept>
#include <base/Logging.hpp>

class TExpressionManager;
class TDLAxiom;
class TDLConceptExpression;
class ReasoningKernel;

namespace owl_om {
namespace restriction {
    enum Type { SELF, VALUE, EXISTS, FORALL, MIN_CARDINALITY, MAX_CARDINALITY, EXACT_CARDINALITY };
}

// Internationalized Resource Identifier
typedef std::string IRI;
typedef std::vector<IRI> IRIList;

class Axiom
{
    TDLAxiom* mAxiom;

public:
    Axiom(TDLAxiom* axiom);

    const TDLAxiom* get() const { return mAxiom; } 
};

class ClassExpression
{
    TDLConceptExpression* mExpression;

public:
    ClassExpression(TDLConceptExpression* expression);

    const TDLConceptExpression* get() const { return mExpression; } 
};

class KnowledgeBase
{
    ReasoningKernel* mKernel;

    TExpressionManager* getExpressionManager();

public:
    enum PropertyType { UNKNOWN_PROPERTY_TYPE, OBJECT, DATA, END_PROPERTY_TYPE };
    enum EntityType { UNKNOWN_ENTITY_TYPE, CLASS, INSTANCE, OBJECT_PROPERTY, DATA_PROPERTY, END_ENTITY_TYPE };

    static std::map<PropertyType, std::string> PropertyTypeTxt;
    static std::map<EntityType, std::string> EntityTypeTxt;

    KnowledgeBase();
    ~KnowledgeBase();

    /**
     * Activate verbose output
     */
    void setVerbose();

    /**
     * Refresh status after adding new updates
     */
    bool refresh();

    // ROLES (PROPERTIES)
    /**
     * Define a transitive object property
     */
    Axiom transitiveProperty(const IRI& property);
    /**
     * Define a reflexive object property
     */
    Axiom reflexiveProperty(const IRI& property);
    /**
     * Define an irreflexive object property 
     */
    Axiom irreflexiveProperty(const IRI& property);
    /**
     * Define a symmetric object property
     */
    Axiom symmetricProperty(const IRI& property);
    /*
     * Define an asymmetric object property
     */
    Axiom asymmetricProperty(const IRI& property);

    // CONCEPTS
    /**
     * Define a subclass relationship between two concepts / classes
     * \param subclass IRI of subclass
     * \param parentClass IRI of parentClass
     * \return corresponding axiom
     */
    Axiom subclassOf(const IRI& subclass, const IRI& parentClass);

    /**
     * Define a subclass relationship between two concepts / classes
     * \param subclass expression for subclass
     * \param parentClass
     * \return corresponding axiom
     */
    Axiom subclassOf(const ClassExpression& subclass, const IRI& parentClass);

    /**
     * Define a subclass relationship between two concepts / classes
     * \param subclass
     * \param parentClass expression for parentClass, e.g. for restrictions
     * \return corresponding axiom
     */
    Axiom subclassOf(const IRI& subclass, const ClassExpression& parentClass);

    /**
     * Define intersection of classes under a given alias
     * \param alias Equivalent concept the intersection of classes
     * \param klass A concept identifier
     * \param otherKlass Another's concept identifier
     * \return corresponding anonymous class expression
     */
    ClassExpression intersectionOf(const IRI& klass, const IRI& otherKlass);

    /**
     * Define intersection of classes under a given alias
     * \param alias Equivalent concept the intersection of classes
     * \param klass A concept identifier
     * \param otherKlass Another's concept identifier
     * \return corresponding anonymous class expression
     */
    ClassExpression intersectionOf(const IRIList& klasses);

    /**
     * Define intersection of classes under a given alias
     * \param alias Equivalent concept the intersection of classes
     * \param klass A concept identifier
     * \param otherKlass Another's concept identifier
     * \return corresponding axiom
     */
    Axiom intersectionOf(const IRI& alias, const ClassExpression& expression);


    /**
     * Define an alias for a given class expression
     * \return corresponding axiom
     */
    Axiom alias(const IRI& alias, const ClassExpression& expression);

    /**
     * Define an instance alias
     * \param alias Alias of instance
     * \param iri Name of entity that should be aliases
     * \param type Entity type that should be aliases
     * \return corresponding axiom
     */
    Axiom alias(const IRI& alias, const IRI& iri, EntityType type);

    /**
     * Define classes to be disjoint
     * \param klass One concept identifier
     * \param another concept identifier
     * \return corresponding axiom
     */
    Axiom disjointClasses(const IRI& klass, const IRI& otherKlass);

    /**
     * Define classes to be disjoint
     * \param List of concepts that will be declared disjoint
     * \return corresponding axiom
     */
    Axiom disjointClasses(const IRIList& klasses);

    /**
     * Define an instance of a concept / class
     * \param instance
     * \param klass
     * \return corresponding axiom
     */
    Axiom instanceOf(const IRI& instance, const IRI& klass);

    /**
     * Define an object relation between two instances / individuals
     * \param instance
     * \param relationProperty the property that related both instances
     * \param otherInstance
     * \return corresponding axiom
     */
    Axiom relatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance);

    /**
     * Define an object relation / role and its domain
     * \param relation
     * \param domain Type of subject for this relation
     */
    Axiom domainOf(const IRI& property, const IRI& domain, PropertyType PropertyType);

    /**
     * Define one of a class relationship
     * \param klassList list of classes
     * \return corresponding class expression
     */
    ClassExpression oneOf(const IRIList& klassList);

    /**
     * Define inverse of a given object property
     */
    Axiom inverseOf(const IRI& baseProperty, const IRI& inverseProperty);

    ClassExpression objectPropertyRestriction(restriction::Type type, const IRI& relationProperty, const IRI& klassOrInstance, int cardinality = -1);

    /**
     * Test if class / concept is a subclass of another
     */
    bool isSubclassOf(const IRI& subclass, const IRI& parentClass);
    /**
     * Test if class expression defines subclass of another
     */
    bool isSubclassOf(const ClassExpression& subclass, const IRI& parentClass);
    /**
     * Test if an instance is of the same type as other instance
     */
    bool isSameClass(const IRI& instance, const IRI& otherInstance) { return typeOf(instance) == typeOf(otherInstance); }

    /**
     * Test if instance is type of a class
     */
    bool isInstanceOf(const IRI& instance, const IRI& klass);
    /**
     * Test if instances are related via given a given property
     */
    bool isRelatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance);

    /**
     * Test if instances are referring to the same
     * \param instance
     * \param otherInstance
     * \return true upon success, false otherwise
     */
    bool isSame(const IRI& instance, const IRI& otherInstance);
   
    /**
     * Retrieve all subclasses of a given klass
     * \return list of all subclasses
     */
    IRIList allSubclassesOf(const IRI& klass, bool direct = false);
    
    /**
     * Retrieve all ancestors of a given klass
     * \return list of all ancestors
     */
    IRIList allAncestorsOf(const IRI& klass, bool direct = false);

    /**
     * Retrieve all known instance of a given klass type
     * \return list of all known instances
     */
    IRIList allInstancesOf(const IRI& klass);

    /**
     * Retrieve all related instances regarding a single instance and a selected property
     * \return list of instances that are related to instance via the relationProperty
     */
    IRIList allRelatedInstances(const IRI& instance, const IRI& relationProperty);

    /**
     * Retrieve all instances regarding a single instance and a selected property
     * \return list of instance that are related to instance via the inverse of the given 
     * relationProperty
     */
    IRIList allInverseRelatedInstances(const IRI& instance, const IRI& relationProperty);

    /**
     * Get all types of a given instance
     * \return list of types of a given instance
     */
    IRIList typesOf(const IRI& instance, bool direct = false);

    /**
     * Get the direct type of a given instance
     * \return klassname for the type of this instance if known
     */
    IRI typeOf(const IRI& instance);

    /**
     * Resolve an alias / instance name
     * \param inverse in order to find an alias for a given instance set to true
     * \return Resolved aliases or a list containing only the instance name if there is no alias
     */
    IRIList getSameAs(const IRI& instanceOrAlias);

    /**
     * Make the list of instances unique, i.e., remove redundant information (aliases)
     */
    IRIList uniqueList(const IRIList& instances);
};

} // namespace owl_om
#endif // OWL_OM_KNOWLEDGE_BASE_HPP
