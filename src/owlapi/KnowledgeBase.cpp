#include "KnowledgeBase.hpp"

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <base/Logging.hpp>

#include <factpp/Kernel.h>
#include <factpp/Actor.h>
#include <factpp/tOntologyPrinterLISP.h>

#include "Vocabulary.hpp"

using namespace owlapi::reasoner::factpp;

namespace owlapi {

std::map<KnowledgeBase::PropertyType, std::string> KnowledgeBase::PropertyTypeTxt = boost::assign::map_list_of
    ( KnowledgeBase::UNKNOWN_PROPERTY_TYPE, "UNKNOWN PROPERTY TYPE")
    ( KnowledgeBase::OBJECT, "OBJECT")
    ( KnowledgeBase::DATA, "DATA");

std::map<KnowledgeBase::EntityType, std::string> KnowledgeBase::EntityTypeTxt = boost::assign::map_list_of
    ( KnowledgeBase::UNKNOWN_ENTITY_TYPE, "UNKNOWN ENTITY TYPE")
    ( KnowledgeBase::CLASS, "CLASS")
    ( KnowledgeBase::INSTANCE, "INSTANCE")
    ( KnowledgeBase::OBJECT_PROPERTY, "OBJECT_RELATION")
    ( KnowledgeBase::DATA_PROPERTY, "DATA_RELATION");


TExpressionManager* KnowledgeBase::getExpressionManager()
{
    return mKernel->getExpressionManager();
}

const TExpressionManager* KnowledgeBase::getExpressionManager() const
{
    return mKernel->getExpressionManager();
}

ClassExpression KnowledgeBase::getClass(const IRI& klass) const
{
    IRIClassExpressionMap::const_iterator cit = mClasses.find(klass);
    if(cit == mClasses.end())
    {
        throw std::invalid_argument("KnowledgeBase::getClass: Class '" + klass.toString() + "' does not exist");
    }

    return cit->second;
}

ClassExpression KnowledgeBase::getClassLazy(const IRI& klass)
{
    try {
        return getClass(klass);
    } catch(const std::invalid_argument& e)
    {
        TDLConceptExpression* f_class = getExpressionManager()->Concept(klass.toString());
        ClassExpression expression(f_class);
        mClasses[klass] = expression;
        return expression;
    }
}

InstanceExpression KnowledgeBase::getInstance(const IRI& instance) const
{
    IRIInstanceExpressionMap::const_iterator cit = mInstances.find(instance);
    if(cit == mInstances.end())
    {
        throw std::invalid_argument("KnowledgeBase::getInstance: Instance '" + instance.toString() + "' does not exist");
    }

    return cit->second;
}

InstanceExpression KnowledgeBase::getInstanceLazy(const IRI& instance)
{
    try {
        return getInstance(instance);
    } catch(const std::invalid_argument& e)
    {
        TDLIndividualExpression* f_individual = getExpressionManager()->Individual(instance.toString());
        InstanceExpression expression(f_individual);
        mInstances[instance] = expression;
        return expression;
    }
}

ObjectPropertyExpression KnowledgeBase::getObjectProperty(const IRI& property) const
{
    IRIObjectPropertyExpressionMap::const_iterator cit = mObjectProperties.find(property);
    if(cit == mObjectProperties.end())
    {
        throw std::invalid_argument("KnowledgeBase::getObjectProperty: ObjectProperty '" + property.toString() + "' does not exist");
    }
    return cit->second;
}

ObjectPropertyExpression KnowledgeBase::getObjectPropertyLazy(const IRI& property)
{
    try {
        return getObjectProperty(property);
    } catch(const std::invalid_argument& e)
    {
        TDLObjectRoleExpression* f_property = getExpressionManager()->ObjectRole(property.toString());
        ObjectPropertyExpression expression(f_property);
        mObjectProperties[property] = expression;
        return expression;
    }
}

IRIList KnowledgeBase::getObjectPropertyDomain(const IRI& property, bool direct) const
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    IRIList classes;

    Actor actor;
    actor.needConcepts();
    mKernel->getORoleDomain( e_property.get(), direct, actor);

    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        classes.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return classes;

}

IRIList KnowledgeBase::getObjectPropertyRange(const IRI& property, bool direct) const
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    IRIList classes;

    Actor actor;
    actor.needConcepts();
    mKernel->getRoleRange( e_property.get(), direct, actor);

    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        classes.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return classes;
}

DataPropertyExpression KnowledgeBase::getDataProperty(const IRI& property) const
{
    IRIDataPropertyExpressionMap::const_iterator cit = mDataProperties.find(property);
    if(cit == mDataProperties.end())
    {
        throw std::invalid_argument("KnowledgeBase::getDataProperty: DataProperty '" + property.toString() + "' does not exist");
    }
    return cit->second;
}

DataPropertyExpression KnowledgeBase::getDataPropertyLazy(const IRI& property)
{
    try {
        return getDataProperty(property);
    } catch(const std::invalid_argument& e)
    {
        TDLDataRoleExpression* f_property = getExpressionManager()->DataRole(property.toString());
        DataPropertyExpression expression(f_property);
        mDataProperties[property] = expression;
        return expression;
    }
}

KnowledgeBase::KnowledgeBase()
    : mKernel(new ReasoningKernel())
{
    mKernel->setVerboseOutput(false);
    mKernel->newKB();
    mKernel->setTopBottomRoleNames ( "http://www.w3.org/2002/07/owl#topObjectProperty", \
        "http://www.w3.org/2002/07/owl#bottomObjectProperty", \
        "http://www.w3.org/2002/07/owl#topDataProperty", \
        "http://www.w3.org/2002/07/owl#bottomDataProperty");

    // registry
    // default data types
    mDataTypes["string"] = getExpressionManager()->getStrDataType();
    mDataTypes["int"] = getExpressionManager()->getIntDataType();
    mDataTypes["double"] = getExpressionManager()->getRealDataType();
    mDataTypes["bool"] = getExpressionManager()->getBoolDataType();
    mDataTypes["time"] = getExpressionManager()->getTimeDataType();

    using namespace owlapi::vocabulary;

    mDataTypes[XSD::resolve("double").toString()] = getExpressionManager()->getRealDataType();
    mDataTypes[XSD::resolve("float").toString()] = getExpressionManager()->getRealDataType();

    mDataTypes[XSD::resolve("long").toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::resolve("int").toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::resolve("short").toString()] = getExpressionManager()->getIntDataType();

    mDataTypes[XSD::nonNegativeInteger().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::nonPositiveInteger().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::positiveInteger().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::negativeInteger().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::integer().toString()] = getExpressionManager()->getIntDataType();

    mDataTypes[XSD::byte().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::unsignedLong().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::unsignedInt().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::unsignedShort().toString()] = getExpressionManager()->getIntDataType();
    mDataTypes[XSD::unsignedByte().toString()] = getExpressionManager()->getIntDataType();

    mDataTypes[XSD::decimal().toString()] = getExpressionManager()->getRealDataType();
    mDataTypes[XSD::string().toString()] = getExpressionManager()->getStrDataType();

    mDataTypes[XSD::dateTime().toString()] = getExpressionManager()->getTimeDataType();
    mDataTypes[XSD::dateTimeStamp().toString()] = getExpressionManager()->getTimeDataType();
}

KnowledgeBase::~KnowledgeBase()
{
    delete mKernel;
}

void KnowledgeBase::setVerbose()
{
    mKernel->setVerboseOutput(true);
}

void KnowledgeBase::refresh()
{
    mKernel->realiseKB();
}

bool KnowledgeBase::isConsistent()
{
    return mKernel->isKBConsistent();
}

void KnowledgeBase::classify()
{
    mKernel->classifyKB();
}

void KnowledgeBase::realize()
{
    mKernel->realiseKB();
}

bool KnowledgeBase::isRealized()
{
    return mKernel->isKBRealised();
}

bool KnowledgeBase::isClassSatifiable(const IRI& klass)
{
    ClassExpression e_klass = getClass(klass);
    return mKernel->isSatisfiable(e_klass.get());
}


ObjectPropertyExpression KnowledgeBase::objectProperty(const IRI& property)
{
    return getObjectPropertyLazy(property);
}

DataPropertyExpression KnowledgeBase::dataProperty(const IRI& property)
{
    return getDataPropertyLazy(property);
}

Axiom KnowledgeBase::transitiveProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectPropertyLazy(property);
    return Axiom( mKernel->setTransitive(e_property.get()) );
}

bool KnowledgeBase::isTransitiveProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    return mKernel->isTransitive(e_property.get());
}

Axiom KnowledgeBase::functionalProperty(const IRI& property, PropertyType type)
{
    switch(type)
    {
        case OBJECT:
        {
            ObjectPropertyExpression e_property = getObjectPropertyLazy(property);
            return Axiom( mKernel->setOFunctional(e_property.get()) );
        }
        case DATA:
        {
            DataPropertyExpression e_property = getDataPropertyLazy(property);
            return Axiom( mKernel->setDFunctional(e_property.get()) );
        }
        default:
            throw std::invalid_argument("KnowledgeBase::functionalProperty: Functional property can only be created for object or data property");
    }
}

bool KnowledgeBase::isFunctionalProperty(const IRI& property)
{
    try {
        ObjectPropertyExpression e_property = getObjectProperty(property);
        return mKernel->isFunctional(e_property.get());
    } catch(...)
    {}

    try {
        DataPropertyExpression e_property = getDataProperty(property);
        return mKernel->isFunctional(e_property.get());
    } catch(...)
    {}

    throw std::invalid_argument("KnowledgeBase::isFunctionalProperty: Property '" + property.toString() + "' is not a known data or object property");
}

Axiom KnowledgeBase::inverseFunctionalProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectPropertyLazy(property);
    return Axiom( mKernel->setInverseFunctional(e_property.get()) );
}

bool KnowledgeBase::isInverseFunctionalProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    return mKernel->isInverseFunctional(e_property.get());
}

Axiom KnowledgeBase::reflexiveProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectPropertyLazy(property);
    return Axiom( mKernel->setReflexive(e_property.get()) );
}

bool KnowledgeBase::isReflexiveProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    return mKernel->isReflexive(e_property.get());
}

Axiom KnowledgeBase::irreflexiveProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectPropertyLazy(property);
    return Axiom( mKernel->setIrreflexive(e_property.get()) );
}

bool KnowledgeBase::isIrreflexiveProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    return mKernel->isIrreflexive(e_property.get());
}

Axiom KnowledgeBase::symmetricProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectPropertyLazy(property);
    return Axiom( mKernel->setSymmetric(e_property.get()) );
}

bool KnowledgeBase::isSymmetricProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    return mKernel->isSymmetric(e_property.get());
}

Axiom KnowledgeBase::asymmetricProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectPropertyLazy(property);
    return Axiom( mKernel->setAsymmetric(e_property.get()) );
}

bool KnowledgeBase::isAsymmetricProperty(const IRI& property)
{
    ObjectPropertyExpression e_property = getObjectProperty(property);
    return mKernel->isAsymmetric(e_property.get());
}

Axiom KnowledgeBase::subclassOf(const IRI& subclass, const IRI& parentClass)
{
    ClassExpression ce = getClassLazy(subclass);
    return subclassOf( ce, parentClass );
}

Axiom KnowledgeBase::subclassOf(const ClassExpression& subclass, const IRI& parentClass)
{
    ClassExpression e_parentClass = getClassLazy(parentClass);
    return Axiom( mKernel->impliesConcepts(subclass.get(), e_parentClass.get()) );
}

Axiom KnowledgeBase::subclassOf(const IRI& subclass, const ClassExpression& parentClass)
{
    ClassExpression e_subclass = getClassLazy(subclass);
    return Axiom( mKernel->impliesConcepts(e_subclass.get(), parentClass.get()) );
}

ClassExpression KnowledgeBase::intersectionOf(const IRI& klass, const IRI& otherKlass)
{
    IRIList klasses;
    klasses.push_back(klass);
    klasses.push_back(otherKlass);
    return intersectionOf(klasses);
}

ClassExpression KnowledgeBase::intersectionOf(const IRIList& klasses)
{
    IRIList::const_iterator cit = klasses.begin();
    getExpressionManager()->newArgList();
    for(; cit != klasses.end(); ++cit)
    {
        ClassExpression e_class = getClassLazy(*cit);
        getExpressionManager()->addArg(e_class.get());
    }
    TDLConceptExpression* f_and = getExpressionManager()->And();
    return ClassExpression( f_and );
}

ClassExpression KnowledgeBase::disjunctionOf(const IRI& klass, const IRI& otherKlass)
{
    IRIList klasses;
    klasses.push_back(klass);
    klasses.push_back(otherKlass);
    return disjunctionOf(klasses);
}

ClassExpression KnowledgeBase::disjunctionOf(const IRIList& klasses)
{
    IRIList::const_iterator cit = klasses.begin();
    getExpressionManager()->newArgList();
    for(; cit != klasses.end(); ++cit)
    {
        ClassExpression e_class = getClassLazy(*cit);
        getExpressionManager()->addArg(e_class.get());
    }
    TDLConceptExpression* f_or = getExpressionManager()->Or();
    return ClassExpression( f_or );
}

Axiom KnowledgeBase::alias(const IRI& alias, const ClassExpression& expression)
{
    ClassExpression e_aliasClass = getClassLazy(alias);

    getExpressionManager()->newArgList();
    getExpressionManager()->addArg(e_aliasClass.get());
    getExpressionManager()->addArg(expression.get());
    return Axiom( mKernel->equalConcepts() );
}

Axiom KnowledgeBase::alias(const IRI& aliasName, const IRI& iri, EntityType type)
{
    switch(type)
    {
        case INSTANCE:
        {
            instanceOf(aliasName, typeOf(iri));

            InstanceExpression e_aliasInstance = getInstanceLazy(aliasName);
            InstanceExpression e_instance = getInstance(iri);

            getExpressionManager()->newArgList();
            getExpressionManager()->addArg(e_aliasInstance.get());
            getExpressionManager()->addArg(e_instance.get());
            return Axiom( mKernel->processSame() );
        }
        case CLASS:
        {
            ClassExpression e_class = getClass(iri);
            return alias(aliasName, e_class);
        }
        default:
            throw std::invalid_argument("KnowledgeBase::alias: Alias for entity type '" + EntityTypeTxt[type] + "' not supported");
    }
}


Axiom KnowledgeBase::disjoint(const IRI& klassOrInstance, const IRI& otherKlassOrInstance, KnowledgeBase::EntityType type)
{
    IRIList klassesOrInstances;
    klassesOrInstances.push_back(klassOrInstance);
    klassesOrInstances.push_back(otherKlassOrInstance);
    return disjoint(klassesOrInstances, type);
}

Axiom KnowledgeBase::disjoint(const IRIList& klassesOrInstances, KnowledgeBase::EntityType type)
{
    IRIList::const_iterator cit = klassesOrInstances.begin();
    getExpressionManager()->newArgList();
    for(; cit != klassesOrInstances.end(); ++cit)
    {
        if(type == CLASS)
        {
            ClassExpression e_class = getClass(*cit);
            getExpressionManager()->addArg(e_class.get());
        } else if(type == INSTANCE)
        {
            InstanceExpression e_instance = getInstance(*cit);
            getExpressionManager()->addArg(e_instance.get());
        }
    }

    if(type == CLASS)
    {
        TDLAxiom* axiom = mKernel->disjointConcepts();
        return Axiom(axiom);
    } else if(type == INSTANCE)
    {
        LOG_WARN("PROCESS DIFFERENT");
        TDLAxiom* axiom = mKernel->processDifferent();
        return Axiom(axiom);
    }

    throw std::runtime_error("owl_om::KnowledgeBase::disjoint requires either list of classes or instances");

}

Axiom KnowledgeBase::instanceOf(const IRI& individual, const IRI& klass)
{
    ClassExpression e_class = getClass(klass);
    InstanceExpression e_instance = getInstanceLazy(individual);
    return Axiom( mKernel->instanceOf(e_instance.get(), e_class.get()) );
}

Axiom KnowledgeBase::relatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance, bool isTrue)
{
    ObjectPropertyExpression e_relation = getObjectPropertyLazy(relationProperty);

    InstanceExpression e_instance = getInstanceLazy(instance);
    InstanceExpression e_otherInstance = getInstanceLazy(otherInstance);

    if(isTrue)
    {
        return Axiom( mKernel->relatedTo(e_instance.get(), e_relation.get(), e_otherInstance.get()) );
    } else {
        return Axiom( mKernel->relatedToNot(e_instance.get(), e_relation.get(), e_otherInstance.get()) );
    }
}

Axiom KnowledgeBase::domainOf(const IRI& property, const IRI& domain, PropertyType propertyType)
{
    switch(propertyType)
    {
        case OBJECT:
        {
            ObjectPropertyExpression e_role = getObjectPropertyLazy(property);
            ClassExpression e_domain = getClassLazy(domain);

            return Axiom( mKernel->setODomain(e_role.get(), e_domain.get()) );
        }
        case DATA:
        {
            DataPropertyExpression e_role = getDataPropertyLazy(property);
            ClassExpression e_domain = getClassLazy(domain);

            return Axiom( mKernel->setDDomain(e_role.get(), e_domain.get()) );
        }
        default:
        {
            throw std::invalid_argument("Invalid property type '" + PropertyTypeTxt[propertyType] + "' for domainOf");
        }
    }
}

Axiom KnowledgeBase::rangeOf(const IRI& property, const IRI& range, PropertyType propertyType)
{
    switch(propertyType)
    {
        case OBJECT:
        {
            ObjectPropertyExpression e_role = getObjectPropertyLazy(property);
            ClassExpression e_range = getClassLazy(range);

            return Axiom( mKernel->setORange(e_role.get(), e_range.get()) );
        }
        case DATA:
        {
            throw std::runtime_error("KnowledgeBase::rangeOf not implemented for data property");
        }
        default:
        {
            throw std::invalid_argument("Invalid property type '" + PropertyTypeTxt[propertyType] + "' for domainOf");
        }
    }
}

Axiom KnowledgeBase::valueOf(const IRI& individual, const IRI& property, const DataValue& dataValue)
{
    TDLAxiom* axiom = mKernel->valueOf( getInstance(individual).get(), getDataProperty(property).get(),dataValue.get());
    return Axiom(axiom);
}

DataValue KnowledgeBase::dataValue(const std::string& value, const std::string& dataType)
{
    IRIDataTypeMap::const_iterator cit = mDataTypes.find(dataType);
    if(cit == mDataTypes.end())
    {
        throw std::invalid_argument("owl_om::KnowledgeBase::dataValue: dataType '" + dataType + "' is unkown");
    }

    DataTypeName dataTypeName = cit->second;
    const TDLDataValue* dataValue = getExpressionManager()->DataValue(value, dataTypeName.get());
    return DataValue(dataValue);
}

Axiom KnowledgeBase::inverseOf(const IRI& base, const IRI& inverse)
{
    ObjectPropertyExpression e_role = getObjectPropertyLazy(base);
    ObjectPropertyExpression e_inverse = getObjectPropertyLazy(inverse);

    return Axiom( mKernel->setInverseRoles(e_role.get(), e_inverse.get()) );
}

ClassExpression KnowledgeBase::oneOf(const IRIList& instanceList)
{
    IRIList::const_iterator cit = instanceList.begin();
    for(; cit != instanceList.end(); ++cit)
    {
        IRI instance = *cit;
        InstanceExpression e_instance = getInstanceLazy(instance);
        getExpressionManager()->newArgList();
        getExpressionManager()->addArg(e_instance.get());
    }
    return ClassExpression( getExpressionManager()->OneOf() );
}

ClassExpression KnowledgeBase::objectPropertyRestriction(restriction::Type type, const IRI& relationProperty, const IRI& klassOrInstance, int cardinality)
{
    ObjectPropertyExpression e_relation = getObjectPropertyLazy(relationProperty);
    switch(type)
    {
        case restriction::SELF:
        {
            return ClassExpression( getExpressionManager()->SelfReference(e_relation.get()) );
        }
        case restriction::VALUE:
        {
            InstanceExpression e_instance = getInstanceLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Value(e_relation.get(), e_instance.get()) );
        }
        case restriction::EXISTS:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Exists(e_relation.get(), e_class.get()) );
        }
        case restriction::FORALL:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Forall(e_relation.get(), e_class.get()) );
        }
        case restriction::MIN_CARDINALITY:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->MinCardinality(cardinality, e_relation.get(), e_class.get()) );
        }
        case restriction::MAX_CARDINALITY:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->MaxCardinality(cardinality, e_relation.get(), e_class.get()) );
        }
        case restriction::EXACT_CARDINALITY:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Cardinality(cardinality, e_relation.get(), e_class.get()) );
        }
    }

    throw std::runtime_error("owl_om::KnowledgeBase::objectPropertyRestriction: Unknown restriction type");
}

bool KnowledgeBase::isSubclassOf(const IRI& subclass, const IRI& parentClass)
{
    ClassExpression e_class = getClass(subclass);
    return isSubclassOf( e_class, parentClass );
}

bool KnowledgeBase::isSubclassOf(const ClassExpression& subclass, const IRI& parentClass)
{
    ClassExpression e_class = getClass(parentClass);
    return mKernel->isSubsumedBy(subclass.get(), e_class.get());
}


bool KnowledgeBase::isEquivalentClass(const IRI& klass0, const IRI& klass1)
{
    ClassExpression e_class0 = getClass(klass0);
    ClassExpression e_class1 = getClass(klass1);

    return mKernel->isEquivalent( e_class0.get(), e_class1.get() );
}

bool KnowledgeBase::isDisjointClass(const IRI& klass0, const IRI& klass1)
{
    ClassExpression e_class0 = getClass(klass0);
    ClassExpression e_class1 = getClass(klass1);

    return mKernel->isDisjoint( e_class0.get(), e_class1.get() );
}

bool KnowledgeBase::isInstanceOf(const IRI& instance, const IRI& klass)
{
    InstanceExpression e_instance = getInstance(instance);
    ClassExpression e_class = getClass(klass);
    return mKernel->isInstance(e_instance.get(), e_class.get());
}

bool KnowledgeBase::isRelatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance)
{
    InstanceExpression e_instance = getInstance(instance);
    InstanceExpression e_otherInstance = getInstance(otherInstance);

    ObjectPropertyExpression e_relation = getObjectProperty(relationProperty);
    return mKernel->isRelated(e_instance.get(), e_relation.get(), e_otherInstance.get());
}

bool KnowledgeBase::isSameInstance(const IRI& instance, const IRI& otherInstance)
{
    InstanceExpression e_instance = getInstance(instance);
    InstanceExpression e_otherInstance = getInstance(otherInstance);
    return mKernel->isSameIndividuals(e_instance.get(), e_otherInstance.get());
}

IRIList KnowledgeBase::allClasses(bool excludeBottomClass) const
{
    IRIList klasses;
    IRIClassExpressionMap::const_iterator cit = mClasses.begin();
    for(; cit != mClasses.end(); ++cit)
    {
        klasses.push_back(cit->first);
    }
    return klasses;
}

IRIList KnowledgeBase::allSubclassesOf(const IRI& klass, bool direct)
{
    ClassExpression e_class = getClass(klass);
    IRIList subclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getSubConcepts(e_class.get(), direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        IRI subclass = IRI::create(result[i]);
        if(subclass != IRI("BOTTOM"))
        {
            subclasses.push_back(subclass);
        }
    }
    delete[] result;
    return subclasses;
}

IRIList KnowledgeBase::allAncestorsOf(const IRI& klass, bool direct)
{
    ClassExpression e_class = getClass(klass);
    IRIList superclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getSupConcepts(e_class.get(), direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        superclasses.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return superclasses;
}

IRIList KnowledgeBase::allEquivalentClasses(const IRI& klass)
{
    ClassExpression e_class = getClass(klass);
    IRIList equivalentclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getEquivalentConcepts(e_class.get(), actor);
    // getSynonyms: 1D NULL terminated array
    const char** result = actor.getSynonyms();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        equivalentclasses.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return equivalentclasses;
}

IRIList KnowledgeBase::allDisjointClasses(const IRI& klass)
{
    ClassExpression e_class = getClass(klass);
    IRIList disjointclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getDisjointConcepts(e_class.get(), actor);
    // getSynonyms: 1D NULL terminated array
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        disjointclasses.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return disjointclasses;

}

IRIList KnowledgeBase::allInstances() const
{
    IRIList instances;
    IRIInstanceExpressionMap::const_iterator cit = mInstances.begin();
    for(; cit != mInstances.end(); ++cit)
    {
        instances.push_back(cit->first);
    }
    return instances;
}

IRIList KnowledgeBase::allInstancesOf(const IRI& klass, bool direct)
{
    ClassExpression e_class = getClass(klass);
    IRIList instances;

    Actor actor;
    actor.needIndividuals();
    mKernel->getInstances(e_class.get(), actor);

    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        // Fact does seem to fail at extracting direct instances
        IRI instanceName = IRI::create(result[i]);
        if(direct && ! (typeOf(instanceName) == klass) )
        {
            continue;
        }
        instances.push_back(instanceName);

    }
    delete[] result;
    return instances;
}

IRIList KnowledgeBase::allRelatedInstances(const IRI& individual, const IRI& relationProperty, const IRI& klass)
{
    IRIList individuals;
    try {
        InstanceExpression e_instance = getInstance(individual);
        ObjectPropertyExpression e_relation = getObjectProperty(relationProperty);

        ReasoningKernel::IndividualSet relatedIndividuals;
        mKernel->getRoleFillers(e_instance.get(), e_relation.get(), relatedIndividuals);

        ReasoningKernel::IndividualSet::const_iterator cit = relatedIndividuals.begin();
        for(; cit != relatedIndividuals.end(); ++cit)
        {
            const TNamedEntry* entry = *cit;
            IRI iri( entry->getName() );

            if( klass.empty() || isInstanceOf(iri, klass) )
            {
                individuals.push_back(iri);
            }
        }
    } catch(const std::exception& e)
    {
        LOG_WARN_S << "'" << individual << "' not related to any via '" << relationProperty << "': " << e.what();
        // There is no such relation defined, thus return an empty list
    }
    return individuals;
}

IRI KnowledgeBase::relatedInstance(const IRI& individual, const IRI& relationProperty, const IRI& klass)
{
    IRIList instances = allRelatedInstances(individual, relationProperty, klass);
    if(instances.empty())
    {
        throw std::invalid_argument("KnowledgeBase::relatedInstance: no instance related to '" + individual.toString() + "' via property '" + relationProperty.toString() + "'");
    }
    return instances.front();
}

IRIList KnowledgeBase::allInverseRelatedInstances(const IRI& individual, const IRI& relationProperty, const IRI& klass)
{
    IRIList individuals;
    try {
        InstanceExpression e_instance = getInstance(individual);
        ObjectPropertyExpression e_relation = getObjectProperty(relationProperty);
        TDLObjectRoleExpression* f_inverse = mKernel->getExpressionManager()->Inverse(e_relation.get());

        ReasoningKernel::IndividualSet relatedIndividuals;
        mKernel->getRoleFillers(e_instance.get(), f_inverse, relatedIndividuals);

        ReasoningKernel::IndividualSet::const_iterator cit = relatedIndividuals.begin();
        for(; cit != relatedIndividuals.end(); ++cit)
        {
            const TNamedEntry* entry = *cit;
            IRI iri( entry->getName() );

            if( klass.empty() || isInstanceOf(iri, klass) )
            {
                individuals.push_back(iri);
            }
        }
    } catch(const std::exception& e)
    {
        LOG_WARN_S << "'" << individual << "' not related to any via '" << relationProperty << "': " << e.what();
        // There is no such relation defined, thus return an empty list
    }
    return individuals;
}

IRIList KnowledgeBase::allObjectProperties() const
{
    IRIList properties;
    IRIObjectPropertyExpressionMap::const_iterator cit = mObjectProperties.begin();
    for(; cit != mObjectProperties.end(); ++cit)
    {
        properties.push_back(cit->first);
    }
    return properties;
}

IRIList KnowledgeBase::allSubObjectProperties(const IRI& propertyRelation, bool direct)
{
    ObjectPropertyExpression e_relation = getObjectProperty(propertyRelation);
    IRIList relations;

    Actor actor;
    actor.needConcepts();
    mKernel->getSubRoles(e_relation.get(), direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        relations.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return relations;
}

IRIList KnowledgeBase::allEquivalentObjectProperties(const IRI& propertyRelation)
{
    ObjectPropertyExpression e_relation = getObjectProperty(propertyRelation);
    IRIList relations;

    Actor actor;
    actor.needConcepts();
    mKernel->getEquivalentRoles(e_relation.get(), actor);
    const char** result = actor.getSynonyms();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        relations.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return relations;
}

bool KnowledgeBase::isSubProperty(const IRI& relationProperty, const IRI& parentRelationProperty)
{
    try {
        return isSubObjectProperty(relationProperty, parentRelationProperty);
    } catch(...)
    {}

    try {
        return isSubDataProperty(relationProperty, parentRelationProperty);
    } catch(...)
    {}

    throw std::invalid_argument("KnowledgeBase::isSubObjectProperty: Property '" + relationProperty.toString() + "' is not a known data or object property");
}

bool KnowledgeBase::isSubObjectProperty(const IRI& relationProperty, const IRI& parentRelationProperty)
{
    ObjectPropertyExpression e_child = getObjectProperty(relationProperty);
    ObjectPropertyExpression e_parent = getObjectProperty(parentRelationProperty);
    return mKernel->isSubRoles(e_child.get(), e_parent.get());
}

bool KnowledgeBase::isSubDataProperty(const IRI& relationProperty, const IRI& parentRelationProperty)
{
    DataPropertyExpression e_child = getDataProperty(relationProperty);
    DataPropertyExpression e_parent = getDataProperty(parentRelationProperty);
    return mKernel->isSubRoles(e_child.get(), e_parent.get());
}

bool KnowledgeBase::isDisjointProperties(const IRI& relationProperty0, const IRI& relationProperty1)
{
    try {
        return isDisjointObjectProperties(relationProperty0, relationProperty1);
    } catch(...)
    {}

    try {
        return isDisjointDataProperties(relationProperty0, relationProperty1);
    } catch(...)
    {}

    throw std::invalid_argument("KnowledgeBase::isDisjointProperties: Property '" + relationProperty0.toString() + "/" + relationProperty1.toString() + "' is not a known data or object property");
}

bool KnowledgeBase::isDisjointObjectProperties(const IRI& relationProperty0, const IRI& relationProperty1)
{
    ObjectPropertyExpression e_child = getObjectProperty(relationProperty0);
    ObjectPropertyExpression e_parent = getObjectProperty(relationProperty1);
    return mKernel->isDisjointRoles(e_child.get(), e_parent.get());
}

bool KnowledgeBase::isDisjointDataProperties(const IRI& relationProperty0, const IRI& relationProperty1)
{
    DataPropertyExpression e_child = getDataProperty(relationProperty0);
    DataPropertyExpression e_parent = getDataProperty(relationProperty1);
    return mKernel->isDisjointRoles(e_child.get(), e_parent.get());
}

IRIList KnowledgeBase::allDataProperties() const
{
    IRIList properties;
    IRIDataPropertyExpressionMap::const_iterator cit = mDataProperties.begin();
    for(; cit != mDataProperties.end(); ++cit)
    {
        properties.push_back(cit->first);
    }
    return properties;
}

IRIList KnowledgeBase::allSubDataProperties(const IRI& propertyRelation, bool direct)
{
    DataPropertyExpression e_relation = getDataProperty(propertyRelation);
    IRIList relations;

    Actor actor;
    actor.needConcepts();
    mKernel->getSubRoles(e_relation.get(), direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        relations.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return relations;
}

IRIList KnowledgeBase::allEquivalentDataProperties(const IRI& propertyRelation)
{
    DataPropertyExpression e_relation = getDataProperty(propertyRelation);
    IRIList relations;

    Actor actor;
    actor.needConcepts();
    mKernel->getEquivalentRoles(e_relation.get(), actor);
    const char** result = actor.getSynonyms();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        relations.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return relations;
}

IRIList KnowledgeBase::getPropertyDomain(const IRI& property, bool direct) const
{
    try {
        return getObjectPropertyDomain(property, direct);
    } catch(...)
    {}

    try {
        return getDataPropertyDomain(property, direct);
    } catch(...)
    {}

    throw std::invalid_argument("KnowledgeBase::isPropertyDomain: Property '" + property.toString() + "' is not a known data or object property");
}

IRIList KnowledgeBase::getDataPropertyDomain(const IRI& property, bool direct) const
{
    DataPropertyExpression e_property = getDataProperty(property);
    IRIList classes;

    Actor actor;
    actor.needConcepts();
    mKernel->getDRoleDomain( e_property.get(), direct, actor);

    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        classes.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return classes;
}

IRIList KnowledgeBase::typesOf(const IRI& instance, bool direct) const
{
    InstanceExpression e_instance = getInstance(instance);

    IRIList klasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getTypes(e_instance.get(), direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        klasses.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    return klasses;
}

IRI KnowledgeBase::typeOf(const IRI& instance) const
{
    IRIList types = typesOf(instance, true);
    assert(!types.empty());
    return types[0];
}

IRIList KnowledgeBase::getSameAs(const IRI& aliasOrInstance)
{
    InstanceExpression e_instance = getInstance(aliasOrInstance);

    IRIList alias;

    Actor actor;
    actor.needIndividuals();
    mKernel->getSameAs(e_instance.get(), actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        alias.push_back( IRI::create(result[i]) );
    }
    delete[] result;
    if(alias.empty())
    {
        IRIList list;
        list.push_back(aliasOrInstance);
        return list;
    } else {
        return alias;
    }
}

IRIList KnowledgeBase::uniqueList(const IRIList& individuals)
{
    IRIList base = individuals;
    IRIList unique = individuals;
    IRIList blacklist;

    IRIList::const_iterator bit = base.begin();

    for(; bit != base.end(); ++bit)
    {
        IRI individual = *bit;
        if( blacklist.end() != std::find(blacklist.begin(), blacklist.end(), individual) )
        {
            LOG_DEBUG_S << "In blacklist: " << individual;
            continue;
        }

        IRIList::iterator uit = unique.begin();
        for(; uit != unique.end();)
        {
            IRI otherIndividual = *uit;
            if(individual != otherIndividual && isSameInstance(individual, otherIndividual) )
            {
                LOG_DEBUG_S << "Remove " << *uit << " since its an alias";
                unique.erase(uit);
                blacklist.push_back(otherIndividual);
            } else {
                ++uit;
            }
        }
    }

    return unique;
}

void KnowledgeBase::retract(Axiom& a)
{
    mKernel->retract(a.get());
}

bool KnowledgeBase::assertAndAddRelation(const IRI& instance, const IRI& relation, const IRI& otherInstance)
{
    if( isRelatedTo(instance, relation, otherInstance) )
    {
        return true;
    } else {
        // seems to be unknown, or false so try add
        Axiom a = relatedTo(instance, relation, otherInstance);
        try {
            refresh();
            LOG_DEBUG_S << "'" << instance << "' '" << relation << "' '" << otherInstance << " is true -- added to db";
            return true;
        } catch(const std::exception& e)
        {
            // ok, that relation has already be added to the knowledge database
            retract(a);
            LOG_DEBUG_S << "'" << instance << "' '" << relation << "' '" << otherInstance << " is false";
            return false;
        }
    }
}

DataValue KnowledgeBase::getDataValue(const IRI& instance, const IRI& dataProperty)
{
    // Iterate overa all axioms and filter the relevant one
    const AxiomVec& axioms = mKernel->getOntology().getAxioms();
    BOOST_FOREACH(TDLAxiom* axiom, axioms)
    {
        TDLAxiomValueOf* valueAxiom = dynamic_cast<TDLAxiomValueOf*>(axiom);
        DataPropertyExpression dataPropertyExpression = getDataProperty(dataProperty);
        if(valueAxiom && dataPropertyExpression.get() == valueAxiom->getAttribute())
        {
            return DataValue( valueAxiom->getValue());
        }
    }

    throw std::runtime_error("KnowledgeBase::getDataValue: instance " + instance.toQuotedString() + " has no value related via " + dataProperty.toQuotedString());

}

std::string KnowledgeBase::toString(representation::Type representation) const
{
    switch(representation)
    {
        case representation::LISP:
        {
            std::stringstream ss;
            TLISPOntologyPrinter printer(ss);

            TOntology& ontology = mKernel->getOntology();
            printer.visitOntology(ontology);

            return ss.str();
        }
        default:
            throw NotSupported("KnowledgeBase::toString: Representation is not supported");
    }

    //TOntology::iterator axiom = getOntology().begin();
    //for(; axiom != ontology.end(); ++axiom)
    //{
    //    axiom->accept(
    //}
}

ExplorationNode KnowledgeBase::getExplorationNode(const IRI& klass)
{
    ClassExpression e_klass = getClass(klass);
    ExplorationNode e( mKernel->buildCompletionTree(e_klass.get()));
    return e;
}

ObjectPropertyExpressionList KnowledgeBase::getRelatedObjectProperties(const IRI& klass)
{
    ExplorationNode e_node = getExplorationNode(klass);
    ReasoningKernel::TCGRoleSet result;
    bool onlyDeterministicInfo = false;
    bool needIncoming = false;

    mKernel->getObjectRoles(e_node.get(), result, onlyDeterministicInfo, needIncoming);

    ObjectPropertyExpressionList relatedObjectProperties;
    ReasoningKernel::TCGRoleSet::const_iterator cit = result.begin();
    for(; cit != result.end(); ++cit)
    {
        TDLRoleExpression* role = const_cast<TDLRoleExpression*>(*cit);
        const ObjectPropertyExpression e_property(dynamic_cast<TDLObjectRoleExpression*>(role));
        relatedObjectProperties.push_back(e_property);
    }

    return relatedObjectProperties;
}

DataPropertyExpressionList KnowledgeBase::getRelatedDataProperties(const IRI& klass)
{
    ExplorationNode e_node = getExplorationNode(klass);
    ReasoningKernel::TCGRoleSet result;
    bool onlyDeterministicInfo = false;
    bool needIncoming = false;

    mKernel->getDataRoles(e_node.get(), result, onlyDeterministicInfo);

    DataPropertyExpressionList relatedDataProperties;
    ReasoningKernel::TCGRoleSet::const_iterator cit = result.begin();
    for(; cit != result.end(); ++cit)
    {
        TDLRoleExpression* role = const_cast<TDLRoleExpression*>(*cit);
        const DataPropertyExpression e_property(dynamic_cast<TDLDataRoleExpression*>(role));
        relatedDataProperties.push_back(e_property);
    }

    return relatedDataProperties;
}

} // end namespace owlapi
