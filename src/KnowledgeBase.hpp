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
class TDLIndividualExpression;
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
    TDLAxiom* get() { return mAxiom; }
};

class ClassExpression
{
    TDLConceptExpression* mExpression;

public:
    ClassExpression(TDLConceptExpression* expression = NULL);

    const TDLConceptExpression* get() const { return mExpression; }
};

class InstanceExpression
{
    TDLIndividualExpression* mExpression;

public:
    InstanceExpression(TDLIndividualExpression* expression = NULL);

    const TDLIndividualExpression* get() const { return mExpression; }
};

typedef std::map<IRI, ClassExpression > IRIClassExpressionMap;
typedef std::map<IRI, InstanceExpression > IRIInstanceExpressionMap;

class KnowledgeBase
{
    ReasoningKernel* mKernel;

    TExpressionManager* getExpressionManager();

    const TExpressionManager* getExpressionManager() const;

    IRIInstanceExpressionMap mInstances;
    IRIClassExpressionMap mClasses;

    bool hasClass(const std::string& klass) const { return mClasses.count(klass); }

    bool hasInstance(const std::string& instance) const { return mInstances.count(instance); }

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
     * \throw std::exception if the data base is inconsistent
     */
    void refresh();

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
     * Define disjunction of classes under a given alias
     * \param alias Equivalent concept the disjunction of classes
     * \param klass A concept identifier
     * \param otherKlass Another's concept identifier
     * \return corresponding anonymous class expression
     */
    ClassExpression disjunctionOf(const IRI& klass, const IRI& otherKlass);

    /**
     * Define disjunction of classes under a given alias
     * \param alias Equivalent concept the disjunction of classes
     * \param klass A concept identifier
     * \param otherKlass Another's concept identifier
     * \return corresponding anonymous class expression
     */
    ClassExpression disjunctionOf(const IRIList& klasses);

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
     * \param klassOrInstance One concept or instance identifier
     * \param another concept or instance identifier
     * \return corresponding axiom
     */
    Axiom disjoint(const IRI& klassOrInstance, const IRI& otherKlassOrInstance, EntityType type);

    /**
     * Define classes to be disjoint / or instance
     * \param klassesOrInstances List of concepts or instances that will be declared disjoint
     * \return corresponding axiom
     */
    Axiom disjoint(const IRIList& klassesOrInstances, EntityType type);

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
     * \param isTrue Set to true if relationship is true, false if is is false
     * \return corresponding axiom
     */
    Axiom relatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance, bool isTrue = true);

    /**
     * Define negated an object relation between two instances / individuals
     * \param instance
     * \param relationProperty the property that related both instances
     * \param otherInstance
     * \return corresponding axiom
     */
    Axiom notRelatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance);


    /**
     * Define an object relation / role and its domain
     * \param relation
     * \param domain Type of subject for this relation
     */
    Axiom domainOf(const IRI& property, const IRI& domain, PropertyType PropertyType);

    /**
     * Define one of a class relationship
     * (enumerated classes): Classes can be described by enumeration of the individuals that make up the class. The members of the class are exactly the set of enumerated individuals; no more, no less. For example, the class of daysOfTheWeek can be described by simply enumerating the individuals Sunday, Monday, Tuesday, Wednesday, Thursday, Friday, Saturday. From this a reasoner can deduce the maximum cardinality (7) of any property that has daysOfTheWeek as its allValuesFrom
     * restriction. 
     *
     * \param instanceList list of classes
     * \return corresponding class expression
     */
    ClassExpression oneOf(const IRIList& instanceList);

    /**
     * Define inverse of a given object property
     */
    Axiom inverseOf(const IRI& baseProperty, const IRI& inverseProperty);

    /**
     * Adds an object property restriction
     * \param type Type, e.g., depending of existential or universal quantifier is required
     * \param relationProperty the property to which the restriction will refer
     * \param klassOrInstance property value
     * \param cardinality if cardinality type is selected this defines the min,max or exact cardinality to use
     * \return corresponding class expression
     */
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
     * \param instance Instance identifier
     * \param klass Class indentifier
     * \return true if instance is of given klass type, false otherwise
     */
    bool isInstanceOf(const IRI& instance, const IRI& klass);

    /**
     * Test if instances are related via given a given property
     * \param instance Instance identifier
     * \param relationProperty relation identifier:
     */
    bool isRelatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance);

    /**
     * Test if instances are referring to the same
     * \param instance
     * \param otherInstance
     * \return true upon success, false otherwise
     */
    bool isSameInstance(const IRI& instance, const IRI& otherInstance);

    /**
     * Get ClassExpression for given IRI if it exists
     * \param klass
     * \throw std::invalid_argument if class does not exist
     */
    ClassExpression getClass(const IRI& klass) const;

    /**
     * Get existing ClassExpression for given IRI or initialize if it does not exist yet
     * \param klass
     */
    ClassExpression getClassLazy(const IRI& klass);

    /**
     * Retrieve all known classes
     * \return list of all classes
     */
    IRIList allClasses(bool excludeBottomClass = true) const;
   
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
     * Retrieve instance  and perform lazy initialization if requested
     * \param instance
     * \param lazyInitialization If set to true will initialized the instance
     * \throw if the instance cannot be found and will not be lazily initalized
     * \return InstanceExpression for that instance
     */
    InstanceExpression getInstance(const IRI& instance) const;

    /**
     * Get existing InstanceExpression for a given IRI or initializ if it does not exist yet
     * \param instance Identifer of instance
     * \return InstanceExpression for that instance
     */
    InstanceExpression getInstanceLazy(const IRI& instance);

    /**
     * Retrieve all known instances
     * \return list of all instances
     */
    IRIList allInstances() const;

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
    IRIList typesOf(const IRI& instance, bool direct = false) const;

    /**
     * Get the direct type of a given instance
     * \return klassname for the type of this instance if known
     */
    IRI typeOf(const IRI& instance) const;

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

    /**
     * Remove axiom from the knowledge base
     */
    void retract(Axiom& a);

    /**
     * Test relation and add result if successfully asserted
     */
    bool assertAndAddRelation(const IRI& instance, const IRI& relation, const IRI& otherInstance);
};

} // namespace owl_om
#endif // OWL_OM_KNOWLEDGE_BASE_HPP
