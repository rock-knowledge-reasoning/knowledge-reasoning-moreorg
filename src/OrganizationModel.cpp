#include "OrganizationModel.hpp"
#include <algorithm>
#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Combinatorics.hpp>
#include <math.h>
#include <set>

#include <owl_om/ccf/CCF.hpp>

using namespace owl_om::vocabulary;

namespace owl_om {

void InterfaceConnection::addParent(const IRI& parent)
{
    parents.push_back(parent);
    if(parents.size() >= 2)
    {
        std::sort(parents.begin(), parents.end());
    }
}

bool InterfaceConnection::sameParents(const InterfaceConnection& other) const
{
    return other.parents == this->parents;
}

bool InterfaceConnection::useSameInterface(const InterfaceConnection& other) const
{
    return other.begin == this->begin || other.begin == this->end || other.end == this->begin || other.end == this->end;
}

bool InterfaceConnection::operator<(const InterfaceConnection& other) const
{
    if(begin < other.begin)
    {
        return true;
    } else if(end < other.end)
    {
        return true;
    }
    return false;

}

bool InterfaceConnection::operator==(const InterfaceConnection& other) const
{
    return end == other.end && begin == other.begin;
}

std::string InterfaceConnection::toString() const
{
    //return "InterfaceConnection from: " + begin.getFragment() + " to: " + end.getFragment() + "\n        parents: " + parents[0].getFragment() + " to " + parents[1].getFragment();
    std::stringstream ss;
    ss << "InterfaceConnection from: " << begin.toString() << " to: " << end.toString();
    //ss << std::endl << "        parents: " << parents[0].getFragment() << " to " << parents[1].getFragment();
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const InterfaceConnection& connection)
{
    os << connection.toString();
    return os;
}

std::string Statistics::toString() const
{
    std::stringstream txt;
    txt << "Organization Model Statistics" << std::endl
        << "    atomic actors:                   " << actorsAtomic << std::endl
        << "    upper bound for combinations:    " << upperCombinationBound << std::endl
        << "    number of inference epochs:      " << numberOfInferenceEpochs << std::endl
        << "    time elapsed in s:               " << timeElapsed.toSeconds() << std::endl
        << "    time composite system gen in s:  " << timeCompositeSystemGeneration.toSeconds() << std::endl
        << "    time registration of comp sys:   " << timeRegisterCompositeSystems.toSeconds() << std::endl
        << "    time for inference:              " << timeInference.toSeconds() << std::endl
        << "    # of interfaces:                 " << interfaces.size() << std::endl
        << "    # of allowed links for comps sys:" << maxAllowedLinks << std::endl
        << "    # of links:                      " << links.size() << std::endl
        << "    # of link combinations:          " << linkCombinations.size() << std::endl
        << "    # of constraints checked:        " << constraintsChecked << std::endl
        << "    # of known actors:               " << actorsKnown.size() << std::endl
        << "    # of inferred actors:            " << actorsInferred.size() << std::endl
        << "    # of composite actors pre:       " << actorsCompositePrevious.size() << std::endl
        << "    # of composite actors post:      " << actorsCompositePost.size() << std::endl
        << "    # of composite actor model pre:  " << actorsCompositeModelPrevious.size() << std::endl
        << "    # of composite actor model post: " << actorsCompositeModelPost.size() << std::endl
        << "    composite actor models post: " << actorsCompositeModelPost << std::endl;

    return txt.str();
}

std::ostream& operator<<(std::ostream& os, const InterfaceConnectionList& list)
{
    InterfaceConnectionList::const_iterator cit = list.begin();
    os << "[" << std::endl;
    for(; cit != list.end(); ++cit)
    {
        os << *cit;
        if(cit + 1 != list.end())
        {
            os << "," << std::endl;
        }
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const InterfaceCombinationList& list)
{
    InterfaceCombinationList::const_iterator cit = list.begin();
    os << "[" << std::endl;
    uint32_t count = 0;
    for(; cit != list.end(); ++cit)
    {
        os << "#" << ++count << ": " << *cit << std::endl;
    }
    os << "]" << std::endl;
    return os;
}

bool Grounding::isComplete() const
{
    RequirementsGrounding::const_iterator mip = mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        if(mip->second == Grounding::ungrounded())
        {
            return false;
        }
    }
    return true;
}

IRIList Grounding::ungroundedRequirements() const
{
    IRIList requirements;
    RequirementsGrounding::const_iterator mip = mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        if(mip->second == Grounding::ungrounded())
        {
            requirements.push_back( mip->second );
        }
    }
    return requirements;
}

std::string Grounding::toString() const
{
    std::stringstream ss;
    ss << "Grounding:" << std::endl;

    RequirementsGrounding::const_iterator mip = mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        ss << "    " << mip->first  << " -> " << mip->second << std::endl;
    }
    return ss.str();
}

OrganizationModel::OrganizationModel(const std::string& filename)
    : mpOntology( new Ontology())
    , mMaximumNumberOfLinks(32)
{
    if(!filename.empty())
    {
        mpOntology = Ontology::fromFile(filename);
        refresh();
    }
}

void OrganizationModel::refresh(bool performInference)
{
    mCurrentStats = Statistics();
    mCurrentStats.timeElapsed = base::Time::now();

    mpOntology->refresh();

    mCurrentStats.upperCombinationBound = upperCombinationBound();

    InterfaceCombinationList interfaceCombinations;
    try {
        interfaceCombinations = generateInterfaceCombinations();
        LOG_WARN_S << "Number of interface combinations: " << interfaceCombinations.size();
    } catch(const std::invalid_argument& e)
    {
        LOG_WARN_S << e.what();
    }

    mCurrentStats.actorsAtomic = mpOntology->allInstancesOf(OM::Actor(), true);
    mCurrentStats.actorsKnown = mpOntology->allInstancesOf(OM::Actor(), false);
    mCurrentStats.actorsCompositePrevious = mpOntology->allInstancesOf(OM::CompositeActor(), true);
    mCurrentStats.actorsCompositeModelPrevious = mpOntology->allInstancesOf(OM::CompositeActorModel(), true);
    mCurrentStats.linkCombinations = interfaceCombinations;

    mCurrentStats.timeRegisterCompositeSystems = base::Time::now();

    IRIList newActors;
    if(!interfaceCombinations.empty())
    {
        InterfaceCombinationList::const_iterator cit = interfaceCombinations.begin();
        std::map<IRISet, uint32_t> actorTypes;
        for(; cit != interfaceCombinations.end(); ++cit)
        {
            IRISet actorCombination;
            InterfaceConnectionList::const_iterator iit = cit->begin();
            for(; iit != cit->end(); ++iit)
            {
                actorCombination.insert(iit->parents.begin(), iit->parents.end());
            }
            uint32_t count = actorTypes[actorCombination] + 1;
            actorTypes[actorCombination] = count;
            IRI newActor = createNewCompositeActor(actorCombination, *cit, count);
            if(!newActor.empty())
            {
                newActors.push_back(newActor);
            }
        }

        mpOntology->refresh();
        LOG_INFO_S << "OrganizationModel: with inferred actors: " << newActors;

        mCurrentStats.actorsInferred = newActors;

    } else {
        LOG_INFO_S << "OrganizationModel: no interface combinations available, so no actors inferred";
    }

    mCurrentStats.timeRegisterCompositeSystems = base::Time::now() - mCurrentStats.timeRegisterCompositeSystems;

    IRIList atomicActors = mpOntology->allInstancesOf(OM::Actor(), true);

    if(performInference)
    {
        mCurrentStats.timeInference = base::Time::now();
        runInferenceEngine();
        mCurrentStats.timeInference = base::Time::now() - mCurrentStats.timeInference;
    }

    mCurrentStats.timeElapsed = base::Time::now() - mCurrentStats.timeElapsed;
    mCurrentStats.actorsCompositePost = mpOntology->allInstancesOf(OM::CompositeActor(), true);
    mCurrentStats.actorsCompositeModelPost = mpOntology->allInstancesOf(OM::CompositeActorModel(), true);

    mStatistics.push_back(mCurrentStats);
}

IRI OrganizationModel::createNewCompositeActor(const IRISet& actorSet, const InterfaceConnectionList& interfaceConnections, uint32_t id)
{
    // create new actor name from fragment, using the first IRI as base
    // add an id, since multiple actor can be combined in different ways
    IRI actorName;
    IRI actorModelName;
    {
        IRISet::const_iterator cit = actorSet.begin();
        for(; cit != actorSet.end(); ++cit)
        {
            if(actorName.empty())
            {
                actorName = *cit;
                actorModelName = getResourceModel(*cit);
            } else {
                actorName = IRI( actorName.toString() + "+" + cit->getFragment());
                actorModelName = IRI( actorModelName.toString() + "+" + getResourceModel(*cit).getFragment());
            }
        }
        std::stringstream ss;
        ss << id;
        actorName = IRI( actorName.toString() + "[" + ss.str() + "]");
        actorModelName = IRI( actorModelName.toString() + "[" + ss.str() + "]");
    }

    try {
        if(mpOntology->isInstanceOf(actorName, OM::CompositeActorModel()))
        {
            // this instance does already exist
            return IRI();
        }
    } catch(const std::invalid_argument& e)
    {
        // actor does not exist
    }

    // Create ActorModel
    mpOntology->instanceOf(actorModelName, OM::CompositeActorModel());

    // Create Instance
    mpOntology->instanceOf(actorName, OM::CompositeActor());
    mpOntology->relatedTo(actorName, OM::modelledBy(), actorModelName);

    IRIList allNewRequirements;
    // Register set of actors which are associate with this actor
    {
        // maker for the set of requirements, since requirement will be all added
        // at this actors level, i.e. per actor a suffix is added
        uint32_t marker = 0;
        IRISet::const_iterator ait = actorSet.begin();
        for(; ait != actorSet.end(); ++ait)
        {
            LOG_DEBUG_S << "New actor '" << actorName << "' has '" << *ait << " with count " << actorSet.size();
            mpOntology->relatedTo(actorName, OM::has(), *ait);

            // ActorModel dependsOn the same requirement instances of the modelled resources, i.e.
            IRIList requirements = allRelatedInstances( getResourceModel(*ait), OM::dependsOn() );
            BOOST_FOREACH(const IRI& requirement, requirements)
            {
                IRI newRequirement = createNewRequirement( requirement, marker);
                mpOntology->relatedTo(actorModelName, OM::dependsOn(), newRequirement );
                allNewRequirements.push_back(newRequirement);
            }
            marker++;
        }
    }
    // allow fast retrieval
    mModelRequirementsCache[actorModelName] = allNewRequirements;

    IRIList usedInterfaces;
    {
        // Associate actor with used interfaces
        InterfaceConnectionList::const_iterator iit = interfaceConnections.begin();
        for(; iit != interfaceConnections.end(); ++iit)
        {

            mpOntology->relatedTo(actorName, OM::uses(), iit->begin);
            mpOntology->relatedTo(actorName, OM::uses(), iit->end);

            usedInterfaces.push_back(iit->begin);
            usedInterfaces.push_back(iit->end);
        }
    }


    LOG_INFO_S << "New actor: '" << actorName << std::endl
        << "     new actor model:    " << actorModelName << std::endl
        << "     involved actors:    " << actorSet << std::endl
        << "     requirements:       " << allNewRequirements << std::endl
        << "     used interfaces:    " << usedInterfaces;

    return actorName;
}

void OrganizationModel::createInstance(const IRI& instanceName, const IRI& klass, const IRI& model)
{
    mpOntology->instanceOf(instanceName, klass);
    mpOntology->relatedTo(instanceName, OM::modelledBy(), model);
}

void OrganizationModel::runInferenceEngine()
{
    IRIList actorModels = mpOntology->allInstancesOf( OM::ActorModel(), false);

    // Load service and capabilities and make sure
    // dependant services end up later in the list
    mServices = mpOntology->allInstancesOf( OM::ServiceModel() );
    mServices = sortByDependency(mServices);

    LOG_WARN_S << "Validate known services: " << mServices;

    mCapabilities = mpOntology->allInstancesOf( OM::CapabilityModel() );
    mCapabilities = sortByDependency(mCapabilities);

    LOG_WARN_S << "Validate known capabilities: " << mCapabilities;
    int count=0;
    int max = actorModels.size();

    BOOST_FOREACH(const IRI& actorModel, actorModels)
    {
        BOOST_FOREACH(const IRI& capability, mCapabilities)
        {
            if( isModelProvider(actorModel, capability) )
            {
                addProvider(actorModel, capability);
            }
        }

        // Check capabilities first, since services might depend upon capabilities
        BOOST_FOREACH(const IRI& service, mServices)
        {
            if( isModelProvider(actorModel, service) )
            {
                addProvider(actorModel, service);
            }
        }


    }

    // Add provides to actors
    IRIList actors = mpOntology->allInstancesOf( OM::Actor(), false);
    BOOST_FOREACH(const IRI& actor, actors)
    {
        IRI model = getResourceModel(actor);

        BOOST_FOREACH(const IRI& capability, mCapabilities)
        {
            if( isModelProvider(model, capability) )
            {
                addProvider(actor, capability);
            }
        }

        BOOST_FOREACH(const IRI& service, mServices)
        {
            if( isModelProvider(model, service) )
            {
                addProvider(actor, service);
            }
        }

    }
}

void OrganizationModel::addProvider(const IRI& actorOrModel, const IRI& model)
{
    mpOntology->relatedTo(actorOrModel, OM::provides(), model);
}

bool OrganizationModel::isProviding(const IRI& actor, const IRI& model) const
{
    std::pair<IRI,IRI> key(actor, model);
    RelationPredicateCache::const_iterator cit = mProviderCache.find(key);
    if(cit != mProviderCache.end())
    {
        return cit->second;
    }

    IRI actorModel = getResourceModel(actor);
    bool providing = mpOntology->isRelatedTo(actorModel, OM::provides(), model);
    mProviderCache[key] = providing;
    return providing;
}

IRIList OrganizationModel::allRelatedInstances(const IRI& instance, const IRI& relation) const
{
    std::pair<IRI,IRI> key(instance, relation);
    RelationCache::const_iterator it = mRelationsCache.find(key);
    if(it != mRelationsCache.end())
    {
        return it->second;
    }

    IRIList all = mpOntology->allRelatedInstances(instance, relation);
    mRelationsCache[key] = all;
    return all;
}

IRIList OrganizationModel::getModelRequirements(const IRI& model) const
{
    IRI2IRIListCache::const_iterator it = mModelRequirementsCache.find(model);
    if(it != mModelRequirementsCache.end())
    {
        return it->second;
    }

    IRI requirementModel = getResourceModel(model);
    IRIList requirements = allRelatedInstances( requirementModel, OM::dependsOn());
    mModelRequirementsCache[model] = requirements;
    return requirements;
}

bool OrganizationModel::isModelProvider(const IRI& actorModel, const IRI& model) const
{
    std::pair<IRI, IRI> key(actorModel, model);
    RelationPredicateCache::const_iterator cit = mModelProviderCache.find(key);
    if(cit != mModelProviderCache.end())
    {
        return cit->second;
    }

    // actorModels define the requirements (that will be fulfilled by instances of this actor model
    IRIList availableResources = getModelRequirements(actorModel);
    IRISet provisioned = mModelProviderSetCache[actorModel];
    availableResources.insert(availableResources.begin(), provisioned.begin(), provisioned.end());

    // model (service/capability/...) define the set of requirements
    IRIList requirements = getModelRequirements(model);

    bool result;
    try {
        Grounding grounding = resolveRequirements(requirements, availableResources, model, actorModel);
        result = grounding.isComplete();

        LOG_DEBUG_S << "Is " << actorModel << " provider for " << model << ": " << grounding.toString();

    } catch(const std::invalid_argument& e)
    {
        result = false;
    }

    mModelProviderCache[key] = result;
    if(result)
    {
        mModelProviderSetCache[actorModel].insert(model);
        LOG_DEBUG_S << "actorModel: " << actorModel << " provides " << model; 
    } else {
        LOG_DEBUG_S << "actorModel: " << actorModel << " does not provide " << model;
    }

    return result;
}

//IRIList OrganizationModel::infer(const IRI& actor, const IRIList& models)
//{
//    IRIList inferred;
//
//    IRI resourceProviderModel = getResourceModel(actor);
//    IRIList availableResources = allRelatedInstances(actor, OM::has());
//    //IRIList availableServices = mpOntology->allRelatedInstances(actor, OM::provides(), OM::ServiceModel());
//    //IRIList availableCapabilities = mpOntology->allRelatedInstances(actor, OM::has(), OM::Capability());
//
//    LOG_DEBUG_S << "Check " << std::endl
//        << "    resourceProvider [" << resourceProviderModel << "] '" << actor  << std::endl
//        << "    available resources:     " << availableResources << std::endl;
//    //    << "    available services:      " << availableServices << std::endl
//    //    << "    available capabilities:  " << availableCapabilities << std::endl;
//
//
//    BOOST_FOREACH(const IRI& model, models)
//    {
//        // If model is not already provided check if its provided now
//        if( !isProviding(actor, model))
//        {
//            IRI modelType = ontology()->typeOf(model);
//
//            LOG_DEBUG_S << "infer " << modelType << " : actor '" << actor << "' resolveRequirements '" << model << "'";
//
//            try {
//                IRIList requirements = getModelRequirements(model);
//                Grounding grounding = resolveRequirements(requirements, availableResources, actor, model);
//                if(grounding.isComplete())
//                {
//                    IRI instance = createNewFromModel(model);
//
//                    addProvider(actor, model);
//                    mpOntology->relatedTo(actor, OM::has(), instance);
//
//                    BOOST_FOREACH(const RequirementsGrounding::value_type& pair, grounding.getRequirementsGrounding())
//                    {
//                        mpOntology->relatedTo(instance, OM::uses(), pair.second);
//                    }
//
//                    LOG_DEBUG_S << modelType << " inference: " << std::endl
//                        << "     actor:     " << actor << std::endl
//                        << "     provides:  " << model << std::endl
//                        << "     has:       " << instance << std::endl;
//
//                    inferred.push_back(model);
//                } else {
//                    LOG_DEBUG_S << "inference: '" << actor << "' does not provide '" << model << "'";
//                }
//            } catch(const std::invalid_argument& e)
//            {
//                LOG_DEBUG_S << "Check failed for '" << model << "' failed " << e.what();
//            }
//        }
//    }
//
//    return inferred;
//}

Grounding OrganizationModel::resolveRequirements(const IRIList& requirements, const IRIList& availableResources, const IRI& resourceProvider, const IRI& requirementModel) const
{
    if(requirements.empty())
    {
        throw std::invalid_argument("OrganizationModel::resolveRequirements: no requirements found");
    }

    LOG_DEBUG_S << "Check " << std::endl
        << "    resourceProvider " << resourceProvider  << std::endl
        << "    requirements for '" << requirementModel << "'" << std::endl
        << "    available resources:     " << availableResources << std::endl
        << "    requirements: " << requirements;

    IRIList::const_iterator cit = requirements.begin();

    bool success = true;
    RequirementsGrounding grounding;
    std::map<IRI,bool> grounded;
    for(; cit != requirements.end(); ++cit)
    {
        IRI requirement = *cit;
        if(availableResources.empty())
        {
            LOG_DEBUG_S << "no available resources on '" << resourceProvider << "'";
        }

        IRIList::const_iterator nit = availableResources.begin();
        bool dependencyFulfilled = false;
        for(; nit != availableResources.end(); ++nit)
        {
            IRI availableResource = *nit;

            if( grounded[availableResource] )
            {
                LOG_DEBUG_S << "resource '" << availableResource << "' already in use (grounded)";
                continue;
            }

            if( isSameResourceModel(requirement, availableResource) )
            {
                LOG_DEBUG_S << "requirement " << requirement << " fulfilled by '" << resourceProvider << "' using '"<< availableResource << "'";
                grounding[requirement] = availableResource;
                grounded[availableResource] = true;
                dependencyFulfilled = true;
                break;
            }
        }

        if(!dependencyFulfilled)
        {
            LOG_DEBUG_S << "requirement " << requirement << " cannot be fulfilled by '" << resourceProvider << "'";
            grounding[requirement] = Grounding::ungrounded();
            success = false;
            break;
        }
    }

    Grounding groundingMap(grounding);

    std::stringstream ss;
    ss << "Fulfillment: ";
    if(success)
    {
        ss << "    [success]" << std::endl;
    } else {
        ss << "    [failed]" << std::endl;
    }

    ss << "    resourceProvider '" << resourceProvider << "'" << std::endl;
    ss << "    requirements for '" << requirementModel << "'" << std::endl;
    ss << groundingMap.toString();

    LOG_DEBUG_S << ss.str();
    return groundingMap;
}

bool OrganizationModel::checkIfCompatible(const IRI& resource, const IRI& otherResource)
{
    std::pair<IRI,IRI> key = std::pair<IRI,IRI>(resource, otherResource);

    RelationPredicateCache::const_iterator cit = mCompatibilityCache.find(key);
    if(cit != mCompatibilityCache.end())
    {
        return cit->second;
    }

    bool isCompatible = mpOntology->isRelatedTo( resource, OM::compatibleWith(), otherResource );
    mCompatibilityCache[key] = isCompatible;
    return isCompatible;
}

IRI OrganizationModel::getResourceModel(const IRI& instance) const
{
    // Try cache first
    IRI2IRICache::const_iterator cit = mResourceModelCache.find(instance);
    if(cit != mResourceModelCache.end())
    {
        return cit->second;
    }

    IRI model;
    try {
         model = mpOntology->relatedInstance(instance, OM::modelledBy(), OM::ResourceModel());
    } catch(const std::invalid_argument& e)
    {
        // no model means, this instance is a model by itself
        model = instance;
    }
    mResourceModelCache[instance] = model;
    return  model;
}

IRI OrganizationModel::getResourceModelInstanceType(const IRI& model) const
{
    IRI modelType;
    try {
        modelType = mpOntology->typeOf(model);
        return mpOntology->relatedInstance(modelType, OM::models(), OM::Resource());
    } catch(const std::invalid_argument& e)
    {
        throw std::invalid_argument("OrganizationModel::createResourceModelInstanceType: '" + modelType.toString() + "' does not specify 'models' relation, thus cannot infer type for new model instances of " + model.toString());
    }
}

IRI OrganizationModel::createNewRequirement(const IRI& requirement, uint32_t marker)
{
    IRI resourceModel = getResourceModel(requirement);

    std::stringstream ss;
    ss << requirement.toString() << marker;

    IRI newRequirement( ss.str() );
    mpOntology->instanceOf( newRequirement, OM::Requirement());
    mpOntology->relatedTo( newRequirement, OM::modelledBy(), resourceModel );
    return newRequirement;
}

IRI OrganizationModel::createNewFromModel(const IRI& model, bool createDependants) const
{
    IRI classType = getResourceModelInstanceType(model);

    LOG_DEBUG_S << "CreateNewFromModel: " << std::endl
        << "    model:    " << model << std::endl
        << "    new class type:    " << classType << std::endl;

    IRIList modelInstances = mpOntology->allInverseRelatedInstances(model, OM::modelledBy());
    std::stringstream ss;
    ss << modelInstances.size();
    IRI newInstanceName = model.toString() + "-instance-" + ss.str();

    mpOntology->instanceOf(newInstanceName, classType);
    mpOntology->relatedTo(newInstanceName, OM::modelledBy(), model);

    if(!createDependants)
    {
        LOG_DEBUG_S << "CreateNewFromModel: " << std::endl
            << "    instance:                 " << newInstanceName << std::endl
            << "    dependant (has relation): [ creation not requested ]";

        return newInstanceName;
    }

    // Create dependencies that come with that model
    IRIList dependencies = getModelRequirements(model);
    IRIList newDependants;
    BOOST_FOREACH(IRI dependency, dependencies)
    {
        // Find out the actual model of this resource (in case in need to instanciate further
        // requirements)
        IRI resourceModel = getResourceModel(dependency);
        if(resourceModel.empty())
        {
            throw std::invalid_argument("owl_om::OrganizationModel::createNewFromModel: could not infer model for dependency / requirement '" + dependency.toString() + "'");
        }

        LOG_DEBUG_S << "CreateNewFromModel: dependency of " << newInstanceName << std::endl
            << "    dependency:        " << dependency << std::endl
            << "    resource model:    " << resourceModel << std::endl;

        // dependency is a placeholder requirement of the same class as the final
        IRI dependant = createNewFromModel(resourceModel, true );
        mpOntology->relatedTo(newInstanceName, OM::has(), dependant);

        newDependants.push_back(dependant);
    }

    LOG_DEBUG_S << "CreateNewFromModel: " << std::endl
        << "    instance:                 " << newInstanceName << std::endl
        << "    dependants (has relation): " << newDependants;

    return newInstanceName;
}

bool OrganizationModel::isSameResourceModel(const IRI& instance, const IRI& otherInstance) const
{
    return getResourceModel(instance) == getResourceModel(otherInstance);
}

InterfaceCombinationList OrganizationModel::generateInterfaceCombinationsCCF()
{
    using namespace base::combinatorics;

    // 1. permute through all interfaces and create 'links'/connections
    //    - make sure constraints hold:
    //        - no self-connection (starting at pointing at same actor)
    //        - compatible interface models
    //        - bidirectional (i.e. A-B equals B-A)
    // 2. use connections to compute combinations of order 1 to N-1, where N is the number of actors
    //    - N-1 since two actors can have only 1 connection
    //

    // Get all basic actors to get number of actors, associated interface and maximum number of connections
    // we have to account for
    IRIList actors = mpOntology->allInstancesOf( OM::Actor(), true );
    if(actors.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough actors for recombination available, i.e. no more than 1");
    }

    IRIList interfaces = mpOntology->allInstancesOf( OM::Interface(), true);
    InterfaceConnectionList validConnections;
    if(interfaces.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough interfaces available, i.e. no more than 1");
    }

    if(actors.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough actors available, i.e. no more than 1");
    }

    LOG_DEBUG_S << "INTERFACES " << interfaces;
    Combination<IRI> interfaceCombinations(interfaces, 2, EXACT);
    do {

        IRIList match = interfaceCombinations.current();
        if(match.size() != 2)
        {
            throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: no two interfaces available to create connection");
        }

        IRIList parents0 = mpOntology->allInverseRelatedInstances(match[0], OM::has());
        IRIList parents1 = mpOntology->allInverseRelatedInstances(match[1], OM::has());

        if(parents0 == parents1)
        {
            // Reject: self connection since start/end belong to the same actor
            continue;
        }

        if( ! checkIfCompatible( getResourceModel(match[0]), getResourceModel(match[1])) )
        {
            // Reject: interfaces are not compatible with each other
            continue;
        }
        // Add connection
        InterfaceConnection connection(match[0], match[1]);
        connection.addParent(parents0.front());
        connection.addParent(parents1.front());

        validConnections.push_back(connection);
    } while( interfaceCombinations.next() );

    mCurrentStats.interfaces = interfaces;
    mCurrentStats.links = validConnections;
    LOG_DEBUG_S << "Valid connections: " << validConnections;


    // Maximum number of connections for a composite actor
    size_t maximumNumberOfConnections =  fmin(actors.size() - 1, mMaximumNumberOfLinks);

    CCF<InterfaceConnection> ccf(validConnections);

    Combination<InterfaceConnection> connectionCombination( validConnections, 2, EXACT);
    do {
        InterfaceConnectionList connectionList = connectionCombination.current();
        LOG_DEBUG_S << "Add p: " << connectionList[0];
        LOG_DEBUG_S << "Add p: " << connectionList[1];
        if( ! ccf.addPositiveConstraint( CCF<InterfaceConnection>::Constraint( connectionList[0] ) ) )
        {
            LOG_DEBUG_S << "Failed to add " << connectionList[0] << "test: " << (connectionList[0] == connectionList[1]) << "size " << ccf.getPositiveConstraints().size();
        }
        if( ! ccf.addPositiveConstraint( CCF<InterfaceConnection>::Constraint( connectionList[1] ) ) )
        {
            LOG_DEBUG_S << "Failed to add " << connectionList[1] << "test: " << (connectionList[0] == connectionList[1]) << "size " << ccf.getPositiveConstraints().size();
        }

        InterfaceConnectionList::const_iterator cit = connectionList.begin();

        uint32_t linkCount = 0;
        std::set<IRI> interfaces;
        std::set<IRI> parents;

        for(; cit != connectionList.end(); ++cit)
        {
            parents.insert(cit->parents[0]);
            parents.insert(cit->parents[1]);
            linkCount++;

            // Check if # involved actors == linkCount + 1, i.e. exactly one connection
            // per pair
            // Makes sure there are no double connections + all nodes are connected to each other
            mCurrentStats.constraintsChecked++;

            // Two links between same parents
            if( parents.size() <= linkCount)
            {
                CCF<InterfaceConnection>::Constraint c;
                c.insert(connectionList[0]);
                c.insert(connectionList[1]);
                ccf.addNegativeConstraint(c);
            }

            mCurrentStats.constraintsChecked++;
            {
                // Check if this interface has already been used
                std::pair< std::set<IRI>::iterator, bool> result = interfaces.insert(cit->begin);
                if(!result.second)
                {
                    CCF<InterfaceConnection>::Constraint c;
                    c.insert(connectionList[0]);
                    c.insert(connectionList[1]);
                    ccf.addNegativeConstraint(c);
                    break;
                }
            }

            mCurrentStats.constraintsChecked++;
            {
                // Check if this interface has already been used
                std::pair< std::set<IRI>::iterator, bool> result = interfaces.insert(cit->end);
                if(!result.second)
                {
                    CCF<InterfaceConnection>::Constraint c;
                    c.insert(connectionList[0]);
                    c.insert(connectionList[1]);
                    ccf.addNegativeConstraint(c);
                    break;
                }
            }
        }
    } while(connectionCombination.next());

    CCF<InterfaceConnection>::Coalitions coalitions;
    //InterfaceConnectionList aStar = ccf.computeConstrainedCoalitions(coalitions);
    LOG_DEBUG_S << "Atoms: " << validConnections;
    LOG_DEBUG_S << "Positive constraints: " << ccf.getPositiveConstraints().toString();
    LOG_DEBUG_S << "Positive constraints #: " << ccf.getPositiveConstraints().size();
    LOG_DEBUG_S << "Negative constraints: " << ccf.getNegativeConstraints().toString();
    LOG_DEBUG_S << "Negative constraints #: " << ccf.getNegativeConstraints().size();
    LOG_DEBUG_S << "Coalitions: " << coalitions.toString();
    //LOG_DEBUG_S << "AStar: " << aStar;

    //CCF<InterfaceConnection>::CoalitionsList list = ccf.createLists(aStar, coalitions);
    //BOOST_FOREACH(CCF<InterfaceConnection>::Coalitions c, list)
    //{
    //    LOG_DEBUG_S << "Coalitions: " << c.toString();
    //}

    //std::vector< CCF<InterfaceConnection>::Coalitions > feasibleCoalitionStructures;
    //ccf.computeFeasibleCoalitions(list, feasibleCoalitionStructures);
    //BOOST_FOREACH( CCF<InterfaceConnection>::Coalitions& structure, feasibleCoalitionStructures)
    //{
    //    LOG_DEBUG_S << "Feasible coalition structure: " << structure.toString();
    //}

    InterfaceCombinationList validCombinations;
    return validCombinations;
}

InterfaceCombinationList OrganizationModel::generateInterfaceCombinations()
{
    using namespace base::combinatorics;

    // 1. permute through all interfaces and create 'links'/connections
    //    - make sure constraints hold:
    //        - no self-connection (starting at pointing at same actor)
    //        - compatible interface models
    //        - bidirectional (i.e. A-B equals B-A)
    // 2. use connections to compute combinations of order 1 to N-1, where N is the number of actors
    //    - N-1 since two actors can have only 1 connection
    //

    // Get all basic actors to get number of actors, associated interface and maximum number of connections
    // we have to account for
    IRIList actors = mpOntology->allInstancesOf( OM::Actor(), true );
    if(actors.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough actors for recombination available, i.e. no more than 1");
    }

    IRIList interfaces = mpOntology->allInstancesOf( OM::Interface(), true);
    InterfaceConnectionList validConnections;
    if(interfaces.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough interfaces available, i.e. no more than 1");
    }

    if(actors.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough actors available, i.e. no more than 1");
    }

    mCurrentStats.timeCompositeSystemGeneration = base::Time::now();
    LOG_DEBUG_S << "Current comp: " << mCurrentStats.timeCompositeSystemGeneration;

    Combination<IRI> interfaceCombinations(interfaces, 2, EXACT);
    do {

        IRIList match = interfaceCombinations.current();
        if(match.size() != 2)
        {
            throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: no two interfaces available to create connection");
        }

        IRIList parents0 = mpOntology->allInverseRelatedInstances(match[0], OM::has());
        IRIList parents1 = mpOntology->allInverseRelatedInstances(match[1], OM::has());

        if(parents0 == parents1)
        {
            // Reject: self connection since start/end belong to the same actor
            continue;
        }

        if( ! checkIfCompatible( getResourceModel(match[0]), getResourceModel(match[1])) )
        {
            // Reject: interfaces are not compatible with each other
            continue;
        }
        // Add connection
        InterfaceConnection connection(match[0], match[1]);
        connection.addParent(parents0.front());
        connection.addParent(parents1.front());

        validConnections.push_back(connection);
    } while( interfaceCombinations.next() );

    mCurrentStats.interfaces = interfaces;
    mCurrentStats.links = validConnections;
    LOG_DEBUG_S << "Valid connections: " << validConnections;


    // Maximum number of connections for a composite actor
    size_t maximumNumberOfConnections =  fmin(actors.size() - 1, mMaximumNumberOfLinks);
    mCurrentStats.maxAllowedLinks = mMaximumNumberOfLinks;

    // Compute valid combinations, i.e.
    //    - each interface is only used once in a combination
    InterfaceCombinationList validCombinations;
    Combination<InterfaceConnection> connectionCombination( validConnections, maximumNumberOfConnections, MAX);
    do {
        // 1. Make sure only one(!) connection exists between two systems
        // 2. Make sure each interface is used only once
        std::set<IRI> interfaces;
        std::set<IRI> parents;
        size_t linkCount = 0;
        bool valid = false;

        InterfaceConnectionList connectionList = connectionCombination.current();
        InterfaceConnectionList::const_iterator cit = connectionList.begin();

        for(; cit != connectionList.end(); ++cit)
        {
            parents.insert(cit->parents[0]);
            parents.insert(cit->parents[1]);
            linkCount++;

            // Check if # involved actors == linkCount + 1, i.e. exactly one connection
            // per pair
            // Makes sure there are no double connections + all nodes are connected to each other
            mCurrentStats.constraintsChecked++;
            if( !(parents.size() - 1 == linkCount) )
            {
                valid = false;
                break;
            }

            mCurrentStats.constraintsChecked++;
            {
                // Check if this interface has already been used
                std::pair< std::set<IRI>::iterator, bool> result = interfaces.insert(cit->begin);
                if(!result.second)
                {
                    valid = false;
                    break;
                }
            }

            mCurrentStats.constraintsChecked++;
            {
                // Check if this interface has already been used
                std::pair< std::set<IRI>::iterator, bool> result = interfaces.insert(cit->end);
                if(!result.second)
                {
                    valid = false;
                    break;
                }
            }

            valid = true;
        }

        if(valid)
        {
            validCombinations.push_back(connectionList);
        }
    } while( connectionCombination.next() );

    mCurrentStats.linkCombinations = validCombinations;
    LOG_DEBUG_S << "Current comp: " << mCurrentStats.timeCompositeSystemGeneration;
    mCurrentStats.timeCompositeSystemGeneration = base::Time::now() - mCurrentStats.timeCompositeSystemGeneration;

    LOG_DEBUG_S << "Valid combinations: " << validCombinations;
    return validCombinations;
}

uint32_t OrganizationModel::upperCombinationBound()
{
    using namespace base::combinatorics;

    IRIList interfaces = mpOntology->allInstancesOf( OM::Interface(), true);
    if(interfaces.size() < 2)
    {
        return 0;
    }

    Combination<IRI> interfaceCombinations(interfaces, 2, EXACT);

    IRIList actors = mpOntology->allInstancesOf( OM::Actor(), true );
    if(actors.size() == 1)
    {
        return 0;
    }

    uint32_t numberOfInterfaceCombinations = interfaceCombinations.numberOfCombinations();
    size_t maximumNumberOfConnections =  actors.size() - 1;

    std::vector<InterfaceConnection> validConnections( numberOfInterfaceCombinations );
    Combination<InterfaceConnection> connectionCombination( validConnections, maximumNumberOfConnections, MAX);
    return connectionCombination.numberOfCombinations();
}

IRI OrganizationModel::getRelatedProviderInstance(const IRI& actor, const IRI& model)
{
   IRIList providers = allRelatedInstances(actor, OM::provides());
   BOOST_FOREACH(const IRI& provider, providers)
   {
       if(getResourceModel(provider) == model)
       {
           LOG_DEBUG_S << "Get related: " << provider << " isModelledBy " << model;
           return provider;
       }
   }

   throw std::invalid_argument("OrganizationModel::getRelatedProviderInstance: " + model.toString() + " is not related to " + actor.toString());
}

void OrganizationModel::setDouble(const IRI& iri, const IRI& dataProperty, double value)
{
    std::stringstream ss;
    ss << value;
    DataValue dataValue = mpOntology->dataValue(ss.str(), "double");
    mpOntology->valueOf(iri, dataProperty, dataValue);
}

IRIList OrganizationModel::sortByDependency(const IRIList& list)
{
    IRIList sortedList;
    BOOST_FOREACH(const IRI& insertItem, list)
    {
        bool inserted = false;
        IRIList::iterator it = sortedList.begin();
        for(; it != sortedList.end(); ++it)
        {
            if( hasModelDependency(*it, insertItem) )
            {
                bool inserted = true;
                sortedList.insert(it, insertItem);
                break;
            }
        }
        if(!inserted)
        {
            sortedList.push_back(insertItem);
        }
    }
    return sortedList;
}

bool OrganizationModel::hasModelDependency(const IRI& main, const IRI& other)
{
    // Check if 'main' has 'other' as dependency, if so return false 
    IRIList mainDependencies = allRelatedInstances(main, OM::dependsOn());
    BOOST_FOREACH(const IRI& dependency, mainDependencies)
    {
        IRI resourceModel = getResourceModel(dependency);
        if(resourceModel == other)
        {
            return true;
        }
    }

    return false;
}

std::ostream& operator<<(std::ostream& os, const Statistics& statistics)
{
    os << statistics.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Statistics>& statisticsList)
{
    os << "StatisticsList: " << std::endl;
    BOOST_FOREACH(const Statistics& s, statisticsList)
    {
        os << s;
    }
    return os;
}

} // end namespace owl_om
