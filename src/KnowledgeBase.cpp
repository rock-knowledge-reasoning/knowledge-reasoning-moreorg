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

TExpressionManager* KnowledgeBase::getExpressionManager()
{
    return mKernel->getExpressionManager();
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

bool KnowledgeBase::refresh()
{
    mKernel->preprocessKB();
    if( !mKernel->isKBConsistent() )
    {
        return false;
    } else {
        mKernel->realiseKB();
    }
    return true;
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
    TDLConceptExpression* f_subclass = getExpressionManager()->Concept(subclass);
    return subclassOf( ClassExpression(f_subclass), parentClass );
}

Axiom KnowledgeBase::subclassOf(const ClassExpression& subclass, const IRI& parentClass)
{
    TDLConceptExpression* f_parentClass = getExpressionManager()->Concept(parentClass);
    return Axiom( mKernel->impliesConcepts(subclass.get(), f_parentClass) );
}

Axiom KnowledgeBase::subclassOf(const IRI& subclass, const ClassExpression& parentClass)
{
    TDLConceptExpression* f_subclass = getExpressionManager()->Concept(subclass);
    return Axiom( mKernel->impliesConcepts(f_subclass, parentClass.get()) );
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
        TDLConceptExpression* f_concept = getExpressionManager()->Concept(*cit);
        getExpressionManager()->addArg(f_concept);
    }
    TDLConceptExpression* f_and = getExpressionManager()->And();
    return ClassExpression( f_and );
}

Axiom KnowledgeBase::alias(const IRI& alias, const ClassExpression& expression)
{
    TDLConceptExpression* f_concept = getExpressionManager()->Concept(alias);

    getExpressionManager()->newArgList();
    getExpressionManager()->addArg(f_concept);
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

            TDLIndividualExpression* f_aliasIndividual = getExpressionManager()->Individual(aliasName);
            TDLIndividualExpression* f_individual = getExpressionManager()->Individual(iri);

            getExpressionManager()->newArgList();
            getExpressionManager()->addArg(f_aliasIndividual);
            getExpressionManager()->addArg(f_individual);
            return Axiom( mKernel->processSame() );
        }
        case CLASS:
        {
            TDLConceptExpression* f_concept = getExpressionManager()->Concept(iri);
            return alias(aliasName, ClassExpression(f_concept) );
        }
        default:
            throw std::invalid_argument("Alias for entity type '" + EntityTypeTxt[type] + "' not supported");
    }
}


Axiom KnowledgeBase::disjointClasses(const IRI& klass, const IRI& otherKlass)
{
    IRIList klasses;
    klasses.push_back(klass);
    klasses.push_back(otherKlass);
    return disjointClasses(klasses);
}

Axiom KnowledgeBase::disjointClasses(const IRIList& klasses)
{
    IRIList::const_iterator cit = klasses.begin();
    getExpressionManager()->newArgList();
    for(; cit != klasses.end(); ++cit)
    {
        TDLConceptExpression* f_concept = getExpressionManager()->Concept(*cit);
        getExpressionManager()->addArg(f_concept);
    }
    TDLAxiom* axiom = mKernel->disjointConcepts();
    return Axiom(axiom);
}

Axiom KnowledgeBase::instanceOf(const IRI& individual, const IRI& klass)
{
    TDLConceptExpression* f_class = getExpressionManager()->Concept(klass);
    TDLIndividualExpression* f_individual = getExpressionManager()->Individual(individual);
    return Axiom( mKernel->instanceOf(f_individual, f_class) );
}

Axiom KnowledgeBase::relatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance)
{
    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);
    TDLIndividualExpression* f_individual = getExpressionManager()->Individual(instance);
    TDLIndividualExpression* f_otherIndividual = getExpressionManager()->Individual(otherInstance);

    return Axiom( mKernel->relatedTo(f_individual, f_relation, f_otherIndividual) );
}

Axiom KnowledgeBase::domainOf(const IRI& property, const IRI& domain, PropertyType propertyType)
{
    switch(propertyType)
    {
        case OBJECT:
        {
            TDLObjectRoleExpression* f_role = getExpressionManager()->ObjectRole(property);
            TDLConceptExpression* f_domain = getExpressionManager()->Concept(domain);

            return Axiom( mKernel->setODomain(f_role, f_domain) );
        }
        case DATA:
        {
            TDLDataRoleExpression* f_role = getExpressionManager()->DataRole(property);
            TDLConceptExpression* f_domain = getExpressionManager()->Concept(domain);

            return Axiom( mKernel->setDDomain(f_role, f_domain) );
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

ClassExpression KnowledgeBase::oneOf(const IRIList& klassList)
{
    IRIList::const_iterator cit = klassList.begin();
    for(; cit != klassList.end(); ++cit)
    {
        IRI klass = *cit;
        TDLConceptExpression* f_concept = getExpressionManager()->Concept(klass);
        getExpressionManager()->newArgList();
        getExpressionManager()->addArg(f_concept);
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
            TDLIndividualExpression* f_individual = getExpressionManager()->Individual(klassOrInstance);
            return ClassExpression( getExpressionManager()->Value(f_relation, f_individual) );
        }
        case restriction::EXISTS:
        {
            TDLConceptExpression* f_concept = getExpressionManager()->Concept(klassOrInstance);
            return ClassExpression( getExpressionManager()->Exists(f_relation, f_concept) );
        }
        case restriction::FORALL:
        {
            TDLConceptExpression* f_concept = getExpressionManager()->Concept(klassOrInstance);
            return ClassExpression( getExpressionManager()->Forall(f_relation, f_concept) );
        }
        case restriction::MIN_CARDINALITY:
        {
            TDLConceptExpression* f_concept = getExpressionManager()->Concept(klassOrInstance);
            return ClassExpression( getExpressionManager()->MinCardinality(cardinality, f_relation, f_concept) );
        }
        case restriction::MAX_CARDINALITY:
        {
            TDLConceptExpression* f_concept = getExpressionManager()->Concept(klassOrInstance);
            return ClassExpression( getExpressionManager()->MaxCardinality(cardinality, f_relation, f_concept) );
        }
        case restriction::EXACT_CARDINALITY:
        {
            TDLConceptExpression* f_concept = getExpressionManager()->Concept(klassOrInstance);
            return ClassExpression( getExpressionManager()->Cardinality(cardinality, f_relation, f_concept) );
        }
    }
}

bool KnowledgeBase::isSubclassOf(const IRI& subclass, const IRI& parentClass)
{
    TDLConceptExpression* f_subclass = getExpressionManager()->Concept(subclass);
    return isSubclassOf( ClassExpression(f_subclass), parentClass );
}

bool KnowledgeBase::isSubclassOf(const ClassExpression& subclass, const IRI& parentClass)
{
    TDLConceptExpression* f_parentClass = getExpressionManager()->Concept(parentClass);
    return mKernel->isSubsumedBy(subclass.get(), f_parentClass);
}

bool KnowledgeBase::isInstanceOf(const IRI& instance, const IRI& klass)
{
    TDLIndividualExpression* f_instance = getExpressionManager()->Individual(instance);
    TDLConceptExpression* f_klass = getExpressionManager()->Concept(klass);
    return mKernel->isInstance(f_instance, f_klass);
}

bool KnowledgeBase::isRelatedTo(const IRI& instance, const IRI& relationProperty, const IRI& otherInstance)
{
    TDLIndividualExpression* f_instance = getExpressionManager()->Individual(instance);
    TDLIndividualExpression* f_otherInstance = getExpressionManager()->Individual(otherInstance);

    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);
    return mKernel->isRelated(f_instance, f_relation, f_otherInstance);
}

bool KnowledgeBase::isSame(const IRI& instance, const IRI& otherInstance)
{
    TDLIndividualExpression* f_instance = getExpressionManager()->Individual(instance);
    TDLIndividualExpression* f_otherInstance = getExpressionManager()->Individual(otherInstance);
    return mKernel->isSameIndividuals(f_instance, f_otherInstance);
}

IRIList KnowledgeBase::allSubclassesOf(const IRI& klass, bool direct)
{
    TDLConceptExpression* f_class = getExpressionManager()->Concept(klass);
    std::vector<std::string> subclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getSubConcepts(f_class, direct, actor);
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
    TDLConceptExpression* f_class = getExpressionManager()->Concept(klass);
    std::vector<std::string> superclasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getSupConcepts(f_class, direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        LOG_DEBUG_S << "super concept of '" << klass << "': " << result[i];
        superclasses.push_back( std::string(result[i]) );
    }
    delete[] result;
    return superclasses;
}

IRIList KnowledgeBase::allInstancesOf(const IRI& klass)
{
    TDLConceptExpression* f_class = getExpressionManager()->Concept(klass);
    std::vector<std::string> instances;

    Actor actor;
    actor.needIndividuals();
    mKernel->getInstances(f_class, actor);
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
    TDLIndividualExpression* f_individual = getExpressionManager()->Individual(individual);
    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);

    ReasoningKernel::IndividualSet relatedIndividuals;
    mKernel->getRoleFillers(f_individual, f_relation, relatedIndividuals);

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
    TDLIndividualExpression* f_individual = getExpressionManager()->Individual(individual);
    TDLObjectRoleExpression* f_relation = getExpressionManager()->ObjectRole(relationProperty);
    TDLObjectRoleExpression* f_inverseRelation = getExpressionManager()->Inverse(f_relation);

    ReasoningKernel::IndividualSet relatedIndividuals;
    mKernel->getRoleFillers(f_individual, f_inverseRelation, relatedIndividuals);

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

IRIList KnowledgeBase::typesOf(const IRI& instance, bool direct)
{
    TDLIndividualExpression* f_instance = getExpressionManager()->Individual(instance);

    IRIList klasses;

    Actor actor;
    actor.needConcepts();
    mKernel->getTypes(f_instance, direct, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        LOG_INFO_S << "instance " << instance << " of type '" << result[i] << "'";
        klasses.push_back( std::string(result[i]) );
    }
    delete[] result;
    return klasses;
}

IRI KnowledgeBase::typeOf(const IRI& instance)
{
    IRIList types = typesOf(instance, true);
    assert(!types.empty());
    return types[0];
}

IRI KnowledgeBase::resolveAlias(const IRI& aliasOrInstance)
{
    TDLIndividualExpression* f_instance = getExpressionManager()->Individual(aliasOrInstance);

    IRIList alias;

    Actor actor;
    actor.needIndividuals();
    mKernel->getSameAs(f_instance, actor);
    const char** result = actor.getElements1D();
    for(size_t i = 0; result[i] != NULL; ++i)
    {
        LOG_INFO_S << "instance or alias of " << aliasOrInstance << " -> '" << result[i] << "'";
        alias.push_back( IRI(result[i]) );
    }
    delete[] result;
    if(alias.empty())
    {
        return aliasOrInstance;
    } else {
       return alias[0];
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
            if(individual != otherIndividual && isSame(individual, otherIndividual) )
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

} // namespace owl_om
