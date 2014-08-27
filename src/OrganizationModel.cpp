#include "OrganizationModel.hpp"
#include <algorithm>
#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Combinatorics.hpp>

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
    return begin < other.begin && end < other.end;

}

std::string InterfaceConnection::toString() const
{
    return "InterfaceConnection from: " + begin.toQuotedString() + " to: " + end.toQuotedString();
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
        << "    upper bound for combinations:  " << upperCombinationBound << std::endl
        << "    number of inference epochs:    " << numberOfInferenceEpochs << std::endl
        << "    time elapsed in s:             " << timeElapsed.toSeconds() << std::endl
        << "    # of interface combinations:   " << interfaceCombinations.size() << std::endl
        << "    # of known actors:             " << actorsKnown.size() << std::endl
        << "    # of inferred actors:          " << actorsInferred.size() << std::endl
        << "    # of composite actors pre:     " << actorsCompositePrevious.size() << std::endl
        << "    # of composite actors post:     " << actorsCompositePost.size() << std::endl;

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

OrganizationModel::OrganizationModel()
    : mpOntology( new Ontology())
{}

OrganizationModel::OrganizationModel(const std::string& filename)
{
    mpOntology = Ontology::fromFile(filename);
    refresh();
}

void OrganizationModel::refresh()
{
    mpOntology->refresh();

    mStats.upperCombinationBound = upperCombinationBound();
    mStats.timeElapsed = base::Time::now();

    InterfaceCombinationList interfaceCombinations;
    try {
        interfaceCombinations = generateInterfaceCombinations();
        LOG_WARN_S << "Number of interface combinations: " << interfaceCombinations.size();
    } catch(const std::invalid_argument& e)
    {
        LOG_WARN_S << e.what();
    }

    mStats.actorsKnown = mpOntology->allInstancesOf(OM::Actor(), false);
    mStats.actorsCompositePrevious = mpOntology->allInstancesOf(OM::CompositeActor(), true);
    mStats.actorsCompositeModelPrevious = mpOntology->allInstancesOf(OM::CompositeActorModel(), true);
    mStats.interfaceCombinations = interfaceCombinations;

    if(!interfaceCombinations.empty())
    {
        InterfaceCombinationList::const_iterator cit = interfaceCombinations.begin();
        std::map<IRISet, uint32_t> actorTypes;
        IRIList newActors;
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
            IRI newActor = createNewActor(actorCombination, *cit, count);
            if(!newActor.empty())
            {
                newActors.push_back(newActor);
            }
        }

        mpOntology->refresh();
        LOG_INFO_S << "OrganizationModel: with inferred actors: " << newActors;

        mStats.actorsInferred = newActors;

    } else {
        LOG_INFO_S << "OrganizationModel: no interface combinations available, so no actors inferred";
    }

    runInferenceEngine();
    mStats.timeElapsed = base::Time::now() - mStats.timeElapsed;
    mStats.actorsCompositePost = mpOntology->allInstancesOf(OM::CompositeActor(), true);
    mStats.actorsCompositeModelPost = mpOntology->allInstancesOf(OM::CompositeActorModel(), true);
}

IRI OrganizationModel::createNewActor(const IRISet& actorSet, const InterfaceConnectionList& interfaceConnections, uint32_t id)
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

    // Register set of actor with this
    {
        IRISet::const_iterator ait = actorSet.begin();
        for(; ait != actorSet.end(); ++ait)
        {
            LOG_DEBUG_S << "New actor '" << actorName << "' has '" << *ait << " with count " << actorSet.size();
            mpOntology->relatedTo(actorName, OM::has(), *ait);

            // ActorModel dependsOn the 'requirement instances of the modelled resources
            //mpOntology->relatedTo(actorModelName, OM::dependsOn(), getResourceModelRequirement(*ait));
        }
    }

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
    IRIList actors = mpOntology->allInstancesOf( OM::Actor() );

    // Infer services
    IRIList services = mpOntology->allInstancesOf( OM::ServiceModel() );
    LOG_DEBUG_S << "Validate known services: " << services;

    IRIList capabilities = mpOntology->allInstancesOf( OM::CapabilityModel() );
    LOG_DEBUG_S << "Validate known capabilities: " << capabilities;

    bool updated = true;
    uint32_t count = 0;
    while(updated)
    {
        LOG_DEBUG_S << "Run inference engine for actors: " << actors;

        IRIList::const_iterator actorIt = actors.begin();
        updated = false;
        for(; actorIt != actors.end(); ++actorIt)
        {
            const IRI& actor = *actorIt;

            {
                IRIList inferred = infer(actor, services);
                if(!inferred.empty())
                {
                    updated = true;
                }
            }
            {
                IRIList inferred = infer(actor, capabilities);
                if(!inferred.empty())
                {
                    updated = true;
                }
            }
        }
    }

    mStats.numberOfInferenceEpochs = count;
}

IRIList OrganizationModel::infer(const IRI& actor, const IRIList& models)
{
    IRIList inferred;

    IRI resourceProviderModel = getResourceModel(actor);
    IRIList availableResources = mpOntology->allRelatedInstances(actor, OM::has());
    IRIList availableServices = mpOntology->allRelatedInstances(actor, OM::provides(), OM::ServiceModel());
    IRIList availableCapabilities = mpOntology->allRelatedInstances(actor, OM::has(), OM::Capability());

    LOG_DEBUG_S << "Check " << std::endl
        << "    resourceProvider [" << resourceProviderModel << "] '" << actor  << std::endl
        << "    available resources:     " << availableResources << std::endl
        << "    available services:      " << availableServices << std::endl
        << "    available capabilities:  " << availableCapabilities << std::endl;

    BOOST_FOREACH(const IRI& model, models)
    {
        // If model is not already provided check if its provided now
        if( !mpOntology->isRelatedTo(actor, OM::provides(), model) )
        {
            IRI modelType = ontology()->typeOf(model);

            LOG_DEBUG_S << "infer " << modelType << " : actor '" << actor << "' resolveRequirements '" << model << "'";

            try {
                Grounding grounding = resolveRequirements(model, availableResources, actor);
                if(grounding.isComplete())
                {
                    IRI instance = createNewFromModel(model);

                    mpOntology->relatedTo(actor, OM::provides(), model);
                    mpOntology->relatedTo(actor, OM::has(), instance);

                    BOOST_FOREACH(const RequirementsGrounding::value_type& pair, grounding.getRequirementsGrounding())
                    {
                        mpOntology->relatedTo(instance, OM::uses(), pair.second);
                    }

                    LOG_DEBUG_S << modelType << " inference: " << std::endl
                        << "     actor:     " << actor << std::endl
                        << "     provides:  " << model << std::endl
                        << "     has:       " << instance << std::endl;

                    inferred.push_back(model);
                } else {
                    LOG_DEBUG_S << "inference: '" << actor << "' does not provide '" << model << "'";
                }
            } catch(const std::invalid_argument& e)
            {
                LOG_DEBUG_S << "Check failed for '" << model << "' failed " << e.what();
            }
        }
    }

    return inferred;
}

Grounding OrganizationModel::resolveRequirements(const IRI& resourceRequirement, const IRIList& availableResources, const IRI& resourceProvider)
{
    IRI requirementModel = getResourceModel(resourceRequirement);
    IRIList requirements = mpOntology->allRelatedInstances( requirementModel, OM::dependsOn(), OM::Requirement());

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
    return mpOntology->isRelatedTo( resource, OM::compatibleWith(), otherResource );
}

IRI OrganizationModel::getResourceModel(const IRI& instance) const
{
    try {
        return mpOntology->relatedInstance(instance, OM::modelledBy(), OM::ResourceModel());
    } catch(const std::invalid_argument& e)
    {
        // no model means, this instance is a model by itself
        return instance;
    }
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
    IRIList dependencies = mpOntology->allRelatedInstances(model, OM::dependsOn(), OM::Requirement());
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

bool OrganizationModel::isSameResourceModel(const IRI& instance, const IRI& otherInstance)
{
    return getResourceModel(instance) == getResourceModel(otherInstance);
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

    LOG_DEBUG_S << "Valid connections: " << validConnections;


    // Maximum number of connections for a composite actor
    size_t maximumNumberOfConnections =  actors.size() - 1;

    // Compute valid combinations, i.e.
    //    - each interface is only used once in a combination
    InterfaceCombinationList validCombinations;
    Combination<InterfaceConnection> connectionCombination( validConnections, maximumNumberOfConnections, MAX);
    do {
        InterfaceConnectionList connectionList = connectionCombination.current();
        InterfaceConnectionList::const_iterator cit = connectionList.begin();
        bool valid = true;

        // 1. Make sure only one(!) connection exists between two systems
        // 2. Make sure each interface is used only once
        //
        // Validate by caching all interfaces that are used by previous connections
        InterfaceConnectionList usedInterfaceConnections;
        for(; cit != connectionList.end(); ++cit)
        {
            InterfaceConnection connection = *cit;

            // check constraints
            InterfaceConnectionList::const_iterator uit = std::find_if(usedInterfaceConnections.begin(), usedInterfaceConnections.end(), boost::bind(&InterfaceConnection::sameParents, &connection, boost::lambda::_1) || boost::bind(&InterfaceConnection::useSameInterface, &connection, boost::lambda::_1));
            if(uit != usedInterfaceConnections.end())
            {
                valid = false;
                break;
            } else {
                // Cache conn
                usedInterfaceConnections.push_back(*cit);
            }
        }
        if(valid)
        {
            validCombinations.push_back(connectionList);
        }
    } while( connectionCombination.next() );

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

}
