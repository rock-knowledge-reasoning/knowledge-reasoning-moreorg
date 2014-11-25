#ifndef OWL_OM_KNOWLEDGE_BASE_HPP
#define OWL_OM_KNOWLEDGE_BASE_HPP

#include <string.h>
#include <map>
#include <vector>
#include <owl_om/Exceptions.hpp>
#include <base/Logging.hpp>
#include <owl_om/reasoner/factpp/Types.hpp>
#include <owl_om/OWLApi.hpp>

namespace owl_om {

namespace representation {
    enum Type { UNKNOWN, LISP };
}

//! Restriction types
namespace restriction {
    enum Type { SELF, VALUE, EXISTS, FORALL, MIN_CARDINALITY, MAX_CARDINALITY, EXACT_CARDINALITY };
}

// Internationalized Resource Identifier -- simply using a string here for simplicity
typedef owlapi::model::IRI IRI;
typedef owlapi::model::IRIList IRIList;
typedef owlapi::model::IRISet IRISet;

typedef std::map<IRI, ClassExpression > IRIClassExpressionMap;
typedef std::map<IRI, InstanceExpression > IRIInstanceExpressionMap;
typedef std::map<IRI, ObjectPropertyExpression > IRIObjectPropertyExpressionMap;
typedef std::map<IRI, DataPropertyExpression > IRIDataPropertyExpressionMap;
typedef std::map<IRI, DataTypeName > IRIDataTypeMap;

enum Representation { UNKNOWN = 0, LISP = 1 };

class KnowledgeBase
{
    ReasoningKernel* mKernel;

    TExpressionManager* getExpressionManager();

    const TExpressionManager* getExpressionManager() const;

    IRIInstanceExpressionMap mInstances;
    IRIClassExpressionMap mClasses;
    IRIObjectPropertyExpressionMap mObjectProperties;
    IRIDataPropertyExpressionMap mDataProperties;
    IRIDataTypeMap mDataTypes;

    bool hasClass(const IRI& klass) const { return mClasses.count(klass); }

    bool hasInstance(const IRI& instance) const { return mInstances.count(instance); }

public:
    enum PropertyType { UNKNOWN_PROPERTY_TYPE, OBJECT, DATA, END_PROPERTY_TYPE };
    enum EntityType { UNKNOWN_ENTITY_TYPE, CLASS, INSTANCE, OBJECT_PROPERTY, DATA_PROPERTY, END_ENTITY_TYPE };

    static std::map<PropertyType, std::string> PropertyTypeTxt;
    static std::map<EntityType, std::string> EntityTypeTxt;

    KnowledgeBase();
    ~KnowledgeBase();

    ReasoningKernel* getReasoningKernel() { return mKernel; }

    /**
     * Activate verbose output
     */
    void setVerbose();

    /**
     * Refresh status after adding new updates
     * \throw std::exception if the data base is inconsistent
     */
    void refresh();

    bool isConsistent();

    void classify();

    void realize();

    bool isRealized();

    bool isClassSatifiable(const IRI& klass);

    // ROLES (PROPERTIES)
    /**
     * Make an object property known
     */
    ObjectPropertyExpression objectProperty(const IRI& property);
    /**
     * Make a data property known
     */
    DataPropertyExpression dataProperty(const IRI& property);
    /**
     * Define a transitive object property
     */
    Axiom transitiveProperty(const IRI& property);
    /**
     * Test if object property is transitive
     */
    bool isTransitiveProperty(const IRI& property);
    /**
     * Define a functional object/data property
     */
    Axiom functionalProperty(const IRI& property, PropertyType type = OBJECT);
    /**
     * Test if property is functional (can be either data or object property)
     */
    bool isFunctionalProperty(const IRI& property);
    /**
     * Define an inverse functional object/data property
     */
    Axiom inverseFunctionalProperty(const IRI& property);
    /**
     * Test if property is inverse functional
     */
    bool isInverseFunctionalProperty(const IRI& property);
    /**
     * Define a reflexive object property
     */
    Axiom reflexiveProperty(const IRI& property);
    /**
     * Test if property is reflexive
     */
    bool isReflexiveProperty(const IRI& property);
    /**
     * Define an irreflexive object property 
     */
    Axiom irreflexiveProperty(const IRI& property);
    /**
     * Test if property is irreflexive
     */
    bool isIrreflexiveProperty(const IRI& property);
    /**
     * Define a symmetric object property
     */
    Axiom symmetricProperty(const IRI& property);
    /**
     * Test if property is symmetric
     */
    bool isSymmetricProperty(const IRI& property);
    /*
     * Define an asymmetric object property
     */
    Axiom asymmetricProperty(const IRI& property);
    /**
     * Test if property is symmetric
     */
    bool isAsymmetricProperty(const IRI& property);

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
     * Define an object relation / role and its range
     * \param relation
     * \param range Type of subject for this relation
     */
    Axiom rangeOf(const IRI& property, const IRI& domain, PropertyType PropertyType);

    /**
     * Define associated value of an individual
     * \param value of
     */
    Axiom valueOf(const IRI& individual, const IRI& property, const DataValue& dataValue);

    /**
     * Define a dataType using a value and the corresponding datatype
     */
    DataValue dataValue(const std::string& value, const std::string& dataType);

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
     * Test if class is equivalent to another
     */
    bool isEquivalentClass(const IRI& klass0, const IRI& klass1);

    /**
     * Test if class is disjoint with another
     */
    bool isDisjointClass(const IRI& klass0, const IRI& klass1);

    /**
     * Test if instance is type of a class
     * \param instance Instance identifier
     * \param klass Class identifier
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
     * Retrieve all equivalent classes
     * \param klass
     * \return list of all equivalent classes
     */
    IRIList allEquivalentClasses(const IRI& klass);

    /**
     * Retrieve all disjoint class
     * \param klass Reference class for this query
     * \return list of classes that are disjoint with the given one
     */
    IRIList allDisjointClasses(const IRI& klass);

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

    ObjectPropertyExpression getObjectProperty(const IRI& property) const;
    ObjectPropertyExpression getObjectPropertyLazy(const IRI& property);

    /**
     * Get property domain
     * \param property Name of property
     * \param direct If direct is true, an exact domain is retrieved if available, otherwise the most specific concept set
     * default is true
     */
    IRIList getObjectPropertyDomain(const IRI& property, bool direct = true) const;

    /**
     * Get property range
     * \param property Name of property
     * \param direct If direct is true, an exact range is retrieved if available, otherwise the most specific concept set
     * default is true
     */
    IRIList getObjectPropertyRange(const IRI& property, bool direct = true) const;

    DataPropertyExpression getDataProperty(const IRI& property) const;
    DataPropertyExpression getDataPropertyLazy(const IRI& property);

    /**
     * Retrieve all known instances
     * \return list of all instances
     */
    IRIList allInstances() const;

    /**
     * Retrieve all known instance of a given klass type
     * \param klass Name of klass
     * \param direct Set to true if only direct instances of the given class shall be retrieved
     * \return list of all known instances
     */
    IRIList allInstancesOf(const IRI& klass, bool direct = false);

    /**
     * Retrieve all related instances regarding a single instance and a selected property
     * which have a certain klass type
     * \param instance Instance name
     * \param relationProperty the relation property
     * \param klass Klass type the relation should map to
     * \return list of instances that are related to instance via the relationProperty
     */
    IRIList allRelatedInstances(const IRI& instance, const IRI& relationProperty, const IRI& klass = IRI());

    /**
     * Retrieve the first found related instance regarding a single instance and a selected property
     * \param instance Instance name
     * \param relationProperty
     * \param klass Klass type the relation should map to
     * \return instance that is related to instance via the relationProperty
     */
    IRI relatedInstance(const IRI& instance, const IRI& relationProperty, const IRI& klass = IRI());

    /**
     * Retrieve all instances regarding a single instance and a selected property
     * \param instance Instance where this relation is bound to
     * \param relationProperty Property for this relation (which will be inverted)
     * \param klass Optional filter klass for the result list, check if result is instance of given klass
     * \return list of instance that are related to instance via the inverse of the given 
     * relationProperty
     */
    IRIList allInverseRelatedInstances(const IRI& instance, const IRI& relationProperty, const IRI& klass = IRI());

    /**
     * Test if given property is a subproperty of a given parent property
     * \return True upon sucess, false otherwise
     */
    bool isSubProperty(const IRI& relationProperty, const IRI& parentRelationProperty);
    /**
     * Test if given properties are disjoint
     */
    bool isDisjointProperties(const IRI& relationProperty0, const IRI& relationProperty1);

    /**
     * Retrieve property domain
     */
    IRIList getPropertyDomain(const IRI& property, bool direct = true) const;

    /**
     * Retrieve all object properties
     */
    IRIList allObjectProperties() const;

    /**
     * Retrieve all sub object properties
     */
    IRIList allSubObjectProperties(const IRI& relationProperty, bool direct = false );

    /**
     * Retrieve all equivalent object properties
     */
    IRIList allEquivalentObjectProperties(const IRI& propertyRelation);

    bool isSubObjectProperty(const IRI& relationProperty, const IRI& parentRelationProperty);
    bool isDisjointObjectProperties(const IRI& relationProperty0, const IRI& relationProperty1);

    /**
     * Retrieve all data properties
     */
    IRIList allDataProperties() const;
    /**
     * Retrieve all sub data properties
     */
    IRIList allSubDataProperties(const IRI& relationProperty, bool direct = false );

    /**
     * Retrieve all equivalent data properties
     */
    IRIList allEquivalentDataProperties(const IRI& propertyRelation);

    bool isSubDataProperty(const IRI& relationProperty, const IRI& parentRelationProperty);
    bool isDisjointDataProperties(const IRI& relationProperty0, const IRI& relationProperty1);

    /**
     * Get property domain
     * \param property Name of property
     * \param direct If direct is true, an exact domain is retrieved if available, otherwise the most specific concept set
     * default is true
     */
    IRIList getDataPropertyDomain(const IRI& property, bool direct = true) const;

    // TODO: isSubPropertyChainOf
    // mKernel->isSubChain( objectproperty ) --> with reference to given argument list

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
     * \return list of unique IRI
     */
    IRIList uniqueList(const IRIList& instances);

    /**
     * Remove axiom from the knowledge base
     */
    void retract(Axiom& a);

    /**
     * Test relation i.e. check if knowledge base remains consistent and add result if successfully asserted
     * \return true if relation has been successfully been added
     */
    bool assertAndAddRelation(const IRI& instance, const IRI& relation, const IRI& otherInstance);

    /**
     * Retrieve data value associated with instance
     * \return data value
     */
    DataValue getDataValue(const IRI& instance, const IRI& dataProperty);

    /**
     * Print the ontology
     * \return 
     */
    std::string toString(representation::Type representation = representation::LISP) const;

    //
    // Exploration
    //
    /**
     * Get node for exploration
     */
    ExplorationNode getExplorationNode(const IRI& klass);

    ObjectPropertyExpressionList getRelatedObjectProperties(const IRI& klass);
    DataPropertyExpressionList getRelatedDataProperties(const IRI& klass);

};

} // namespace owl_om
#endif // OWL_OM_KNOWLEDGE_BASE_HPP
