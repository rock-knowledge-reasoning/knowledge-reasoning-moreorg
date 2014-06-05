#include "KnowledgeBase.hpp"

#include <factpp/Kernel.h>
#include <factpp/Actor.h>
#include <boost/assign/list_of.hpp>
#include <base/Logging.hpp>

namespace owl_om {

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


Axiom::Axiom(TDLAxiom* axiom)
    : mAxiom(axiom)
{
}

ClassExpression::ClassExpression(TDLConceptExpression* expression)
    : mExpression(expression)
{}

InstanceExpression::InstanceExpression(TDLIndividualExpression* expression)
    : mExpression(expression)
{}

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
        throw std::invalid_argument("Class '" + klass + "' does not exist");
    }

    return cit->second;
}

ClassExpression KnowledgeBase::getClassLazy(const IRI& klass)
{
    try {
        return getClass(klass);
    } catch(const std::invalid_argument& e)
    {
        TDLConceptExpression* f_class = getExpressionManager()->Concept(klass);
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
        throw std::invalid_argument("Instance '" + instance + "' does not exist");
    }

    return cit->second;
}

InstanceExpression KnowledgeBase::getInstanceLazy(const IRI& instance)
{
    try {
        return getInstance(instance);
    } catch(const std::invalid_argument& e)
    {
        TDLIndividualExpression* f_individual = getExpressionManager()->Individual(instance);
        InstanceExpression expression(f_individual);
        mInstances[instance] = expression;
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

Axiom KnowledgeBase::transitiveProperty(const IRI& property)
{
    TDLObjectRoleExpression* f_property = getExpressionManager()->ObjectRole(property);
    return Axiom( mKernel->setTransitive(f_property) );
}

Axiom KnowledgeBase::reflexiveProperty(const IRI& property)
{
    TDLObjectRoleExpression* f_property = getExpressionManager()->ObjectRole(property);
    return Axiom( mKernel->setReflexive(f_property) );
}

Axiom KnowledgeBase::irreflexiveProperty(const IRI& property)
{
    TDLObjectRoleExpression* f_property = getExpressionManager()->ObjectRole(property);
    return Axiom( mKernel->setIrreflexive(f_property) );
}

Axiom KnowledgeBase::symmetricProperty(const IRI& property)
{
    TDLObjectRoleExpression* f_property = getExpressionManager()->ObjectRole(property);
    return Axiom( mKernel->setSymmetric(f_property) );
}

Axiom KnowledgeBase::asymmetricProperty(const IRI& property)
{
    TDLObjectRoleExpression* f_property = getExpressionManager()->ObjectRole(property);
    return Axiom( mKernel->setAsymmetric(f_property) );
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
            throw std::invalid_argument("Alias for entity type '" + EntityTypeTxt[type] + "' not supported");
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
    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);

    InstanceExpression e_instance = getInstanceLazy(instance);
    InstanceExpression e_otherInstance = getInstanceLazy(otherInstance);

    if(isTrue)
    {
        return Axiom( mKernel->relatedTo(e_instance.get(), f_relation, e_otherInstance.get()) );
    } else {
        return Axiom( mKernel->relatedToNot(e_instance.get(), f_relation, e_otherInstance.get()) );
    }
}

Axiom KnowledgeBase::domainOf(const IRI& property, const IRI& domain, PropertyType propertyType)
{
    switch(propertyType)
    {
        case OBJECT:
        {
            TDLObjectRoleExpression* f_role = getExpressionManager()->ObjectRole(property);
            ClassExpression e_domain = getClassLazy(domain);

            return Axiom( mKernel->setODomain(f_role, e_domain.get()) );
        }
        case DATA:
        {
            TDLDataRoleExpression* f_role = getExpressionManager()->DataRole(property);
            ClassExpression e_domain = getClassLazy(domain);

            return Axiom( mKernel->setDDomain(f_role, e_domain.get()) );
        }
        default:
        {
            throw std::invalid_argument("Invalid property type '" + PropertyTypeTxt[propertyType] + "' for domainOf");
        }
    }
}

Axiom KnowledgeBase::inverseOf(const IRI& base, const IRI& inverse)
{
    TDLObjectRoleExpression* f_role = getExpressionManager()->ObjectRole(base);
    TDLObjectRoleExpression* f_inverse = getExpressionManager()->ObjectRole(inverse);

    return Axiom( mKernel->setInverseRoles(f_role, f_inverse) );
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
    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);
    switch(type)
    {
        case restriction::SELF:
        {
            return ClassExpression( getExpressionManager()->SelfReference(f_relation) );
        }
        case restriction::VALUE:
        {
            InstanceExpression e_instance = getInstanceLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Value(f_relation, e_instance.get()) );
        }
        case restriction::EXISTS:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Exists(f_relation, e_class.get()) );
        }
        case restriction::FORALL:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Forall(f_relation, e_class.get()) );
        }
        case restriction::MIN_CARDINALITY:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->MinCardinality(cardinality, f_relation, e_class.get()) );
        }
        case restriction::MAX_CARDINALITY:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->MaxCardinality(cardinality, f_relation, e_class.get()) );
        }
        case restriction::EXACT_CARDINALITY:
        {
            ClassExpression e_class = getClassLazy(klassOrInstance);
            return ClassExpression( getExpressionManager()->Cardinality(cardinality, f_relation, e_class.get()) );
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

    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);
    return mKernel->isRelated(e_instance.get(), f_relation, e_otherInstance.get());
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
    std::vector<std::string> subclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getSubConcepts(e_class.get(), direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        LOG_DEBUG_S << "subconcept of '" << klass << "': " << result[i];
        subclasses.push_back( std::string(result[i]) );
    }
    delete[] result;
    return subclasses;
}

IRIList KnowledgeBase::allAncestorsOf(const IRI& klass, bool direct)
{
    ClassExpression e_class = getClass(klass);
    std::vector<std::string> superclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getSupConcepts(e_class.get(), direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        LOG_DEBUG_S << "super concept of '" << klass << "': " << result[i];
        superclasses.push_back( std::string(result[i]) );
    }
    delete[] result;
    return superclasses;
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

IRIList KnowledgeBase::allInstancesOf(const IRI& klass)
{
    ClassExpression e_class = getClass(klass);
    std::vector<std::string> instances;

    Actor actor;
    actor.needIndividuals();
    mKernel->getInstances(e_class.get(), actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        LOG_DEBUG_S << "instance of '" << klass << "': " << result[i];
        instances.push_back( std::string(result[i]) );
    }
    delete[] result;
    return instances;
}

IRIList KnowledgeBase::allRelatedInstances(const IRI& individual, const IRI& relationProperty)
{
    InstanceExpression e_instance = getInstance(individual);
    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);

    ReasoningKernel::IndividualSet relatedIndividuals;
    mKernel->getRoleFillers(e_instance.get(), f_relation, relatedIndividuals);

    IRIList individuals;
    ReasoningKernel::IndividualSet::const_iterator cit = relatedIndividuals.begin();
    for(; cit != relatedIndividuals.end(); ++cit)
    {
        const TNamedEntry* entry = *cit;
        LOG_INFO_S << "'" << individual << "' " << relationProperty << " '" << entry->getName() << "'";
        individuals.push_back( std::string( entry->getName() ) );
    }
    return individuals;
}

IRIList KnowledgeBase::allInverseRelatedInstances(const IRI& individual, const IRI& relationProperty)
{
    InstanceExpression e_instance = getInstance(individual);
    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);
    TDLObjectRoleExpression* f_inverseRelation = getExpressionManager()->Inverse(f_relation);

    ReasoningKernel::IndividualSet relatedIndividuals;
    mKernel->getRoleFillers(e_instance.get(), f_inverseRelation, relatedIndividuals);

    std::vector<std::string> individuals;
    ReasoningKernel::IndividualSet::const_iterator cit = relatedIndividuals.begin();
    for(; cit != relatedIndividuals.end(); ++cit)
    {
        const TNamedEntry* entry = *cit;
        LOG_INFO_S << "'" << individual << "' -" << relationProperty << " '" << entry->getName() << "'";
        individuals.push_back( std::string( entry->getName() ) );
    }
    return individuals;
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
        LOG_INFO_S << "instance " << instance << " of type '" << result[i] << "'";
        klasses.push_back( std::string(result[i]) );
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
        LOG_INFO_S << "instance or alias of " << aliasOrInstance << " -> '" << result[i] << "'";
        alias.push_back( IRI(result[i]) );
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

} // namespace owl_om
