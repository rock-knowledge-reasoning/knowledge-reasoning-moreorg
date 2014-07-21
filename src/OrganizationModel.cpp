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
    LOG_DEBUG_S << "Use same interface of " << *this << " against " << other;
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

OrganizationModel::OrganizationModel()
    : mpOntology( new Ontology())
{}

OrganizationModel::OrganizationModel(const std::string& filename, bool runInference)
{
    mpOntology = Ontology::fromFile(filename);
    mpOntology->refresh();

    InterfaceCombinationList interfaceCombinations = generateInterfaceCombinations();
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
        newActors.push_back(newActor);
    }
    mpOntology->refresh();
    LOG_DEBUG_S << "OrganizationModel: with inferred actors: " << newActors;

    if(runInference)
    {
        runInferenceEngine();
    }
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

    // Create ActorModel
    mpOntology->instanceOf(actorModelName, OM::ActorModel());

    // Create Instance
    mpOntology->subclassOf(OM::CompositeActor(), OM::Actor());
    mpOntology->instanceOf(actorName, OM::CompositeActor());
    mpOntology->relatedTo(actorName, OM::modelledBy(), actorModelName);

    // Register set of actor with this
    {
        IRISet::const_iterator ait = actorSet.begin();
        for(; ait != actorSet.end(); ++ait)
        {
            LOG_DEBUG_S << "New actor '" << actorName << "' has '" << *ait << " with count " << actorSet.size();
            mpOntology->relatedTo(actorName, OM::has(), *ait);
        }
    }

    IRIList usedInterfaces;
    // TODO: add depend_on dependency for actor model resources
    {
        // mpOntology->relatedTo(actorClassFromRecombination, OM::dependsOn(), getResourceModel(actor));
        // mpOntology->relatedTo(actorClassFromRecombination, OM::dependsOn(), getResourceModel(*cit));

        InterfaceConnectionList::const_iterator iit = interfaceConnections.begin();
        for(; iit != interfaceConnections.end(); ++iit)
        {
            mpOntology->relatedTo(actorModelName, OM::uses(), iit->begin);
            mpOntology->relatedTo(actorModelName, OM::uses(), iit->end);

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
    IRIList services = mpOntology->allInstancesOf( OM::ServiceModel() );
    IRIList::const_iterator actorIt = actors.begin();

    LOG_INFO_S << "Run inference engine: # of actors: '" << actors.size();

    bool updated = true;
    while(updated)
    {
        updated = false;
        for(; actorIt != actors.end(); ++actorIt)
        {
            IRIList::const_iterator serviceIt = services.begin();
            for(; serviceIt != services.end(); ++serviceIt)
            {
                LOG_INFO_S << "inference: '" << *actorIt << "' checkIfFulfills? '" << *serviceIt << "'";
                if( checkIfFulfills(*actorIt, *serviceIt) )
                {
                    LOG_INFO_S << "inference: '" << *actorIt << "' provides '" << *serviceIt << "'";
                    mpOntology->relatedTo(*actorIt, OM::provides(), *serviceIt);
                    updated = true;
                } else {
                    LOG_INFO_S << "inference: '" << *actorIt << "' does not provide '" << *serviceIt << "'";
                }
            }
        }
    }
}

bool OrganizationModel::checkIfFulfills(const IRI& resourceProvider, const IRI& resourceRequirements)
{
    IRI resourceProviderModel = getResourceModel(resourceProvider);

    IRIList availableResources = mpOntology->allRelatedInstances(resourceProvider, OM::has());
    IRIList availableServices = mpOntology->allRelatedInstances(resourceProviderModel, OM::provides());

    IRI requirementModel = getResourceModel(resourceRequirements);
    IRIList requirements = mpOntology->allRelatedInstances( requirementModel, OM::dependsOn());
    std::map<IRI,IRI> grounding;

    LOG_INFO_S << "Check " << std::endl
        << "    resourceProvider [" << resourceProviderModel << "] '" << resourceProvider  << std::endl
        << "    requirements for '" << requirementModel << "'" << std::endl
        << "    available resources: " << availableResources << std::endl
        << "    available services: " << availableServices << std::endl
        << "    requirements: " << requirements;

    IRIList::const_iterator cit = requirements.begin();
    bool success = true;
    for(; cit != requirements.end(); ++cit)
    {
        IRI requirement = *cit;
        if(availableResources.empty())
        {
            LOG_DEBUG_S << "no available resources on '" << resourceProvider << "'";
        }
        IRIList::iterator nit = availableResources.begin();
        bool dependencyFulfilled = false;
        for(; nit != availableResources.end(); ++nit)
        {
            IRI availableResource = *nit;
            if( isSameResourceModel(requirement, availableResource) )
            {
                LOG_INFO_S << "requirement " << requirement << " fulfilled by '" << resourceProvider << "' using '"<< availableResource << "'";
                grounding[requirement] = availableResource;
                dependencyFulfilled = true;
                break;
            }
        }

        IRI resourceModel = getResourceModel(requirement);
        LOG_DEBUG_S << "Search for service: '" << resourceModel << "'";
        IRIList::iterator sit = std::find(availableServices.begin(), availableServices.end(), resourceModel);
        if(sit != availableServices.end())
        {
            IRI availableService = *sit;
            LOG_INFO_S << "requirement " << requirement << " fulfilled by service " << availableService;
            grounding[requirement] = availableService;
            dependencyFulfilled = true;
        }

        if(!dependencyFulfilled)
        {
            LOG_INFO_S << "requirement " << requirement << " cannot be fulfilled by service '" << resourceProvider << "'";
            grounding[requirement] = IRI("?");
            success = false;
            break;
        }
    }

    std::map<IRI,IRI>::const_iterator mip = grounding.begin();
    std::stringstream ss;
    ss << "Fulfillment: ";
    if(success)
    {
        ss << "    [success]" << std::endl;
    } else {
        ss << "    [failed]" << std::endl;
    }

    ss << "    resourceProvider [" << resourceProviderModel << "] '" << resourceProvider  << std::endl;
    ss << "    requirements for '" << requirementModel << "'" << std::endl;

    for(; mip != grounding.end(); ++mip)
    {
        ss << "    " << mip->first  << " -> " << mip->second << std::endl;
    }
    LOG_INFO_S << ss.str();
    return success;
}

bool OrganizationModel::checkIfCompatible(const IRI& resource, const IRI& otherResource)
{
    IRIList resourceInterfaces;
    IRIList otherResourceInterfaces;

    // First extract available interfaces from the associated models
    {
        IRIList interfaces = mpOntology->allRelatedInstances(resource, OM::has());
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mpOntology->isInstanceOf(*cit, OM::Interface()) )
            {
                LOG_INFO_S << resource << " has (Interface) " << *cit;
                resourceInterfaces.push_back(*cit);
            }
        }
    }
    {
        IRIList interfaces = mpOntology->allRelatedInstances(otherResource, OM::has());
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mpOntology->isInstanceOf(*cit, OM::Interface()) )
            {
                LOG_INFO_S << otherResource << " has (Interface) " << *cit;
                otherResourceInterfaces.push_back(*cit);
            }
        }
    }


    LOG_DEBUG_S << "Check compatibility of '" << resource << " with '" << otherResource << "'" << std::endl
        << "    interfaces of first: " << resourceInterfaces << std::endl
        << "    interfaces of second: " << otherResourceInterfaces << std::endl;

    // For each interface check if the models are compatible to each other
    IRIList::const_iterator rit = resourceInterfaces.begin();
    for(; rit != resourceInterfaces.end(); ++rit)
    {
        IRI interface = *rit;
        IRI interfaceModel = getResourceModel(interface);

        IRIList::const_iterator oit = otherResourceInterfaces.begin();
        for(; oit != otherResourceInterfaces.end(); ++oit)
        {
            IRI otherInterface = *oit;
            IRI otherInterfaceModel = getResourceModel(otherInterface);

            if( mpOntology->isRelatedTo(interfaceModel, OM::compatibleWith(), otherInterfaceModel) )
            {
                LOG_INFO_S << resource << " compatibleWith " << otherResource << " via " << interface << "[" << interfaceModel << "] and " << otherInterface << "[" << otherInterfaceModel << "]";
                return true;
            }
        }
    }

    LOG_INFO_S << resource << " is not compatibleWith " << otherResource << " via any interface";
    return false;
}

CandidatesList OrganizationModel::checkIfCompatibleNow(const IRI& instance, const IRI& otherInstance)
{
    IRIList usedInterfaces;

    IRIList resourceInterfaces;
    IRIList otherResourceInterfaces;

    IRIList resourceUsedInstances = mpOntology->allRelatedInstances(instance, OM::uses());
    IRIList otherResourceUsedInstances = mpOntology->allRelatedInstances(otherInstance, OM::uses());

    {
        // Identify the list of unused interfaces for 'instance'
        IRIList interfaces = mpOntology->allRelatedInstances( instance, OM::has());
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mpOntology->isInstanceOf(*cit, OM::Interface()) )
            {
                IRIList::iterator uit = std::find(resourceUsedInstances.begin(), resourceUsedInstances.end(), *cit);
                if( uit != resourceUsedInstances.end())
                {
                    LOG_INFO_S << instance << " uses (Interface) " << *cit << " already";
                } else {
                    LOG_INFO_S << instance << " has unused (Interface) " << *cit;
                    resourceInterfaces.push_back(*cit);
                }
            }
        }
    }
    {
        // Identify the list of unused interfaces for 'otherInstance'
        IRIList interfaces = mpOntology->allRelatedInstances( otherInstance, OM::has());
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mpOntology->isInstanceOf(*cit, OM::Interface()) )
            {
                IRIList::iterator uit = std::find(otherResourceUsedInstances.begin(), otherResourceUsedInstances.end(), *cit);
                if( uit != otherResourceUsedInstances.end())
                {
                    LOG_INFO_S << otherInstance << " uses (Interface) " << *cit << " already";
                } else {
                    LOG_INFO_S << otherInstance << " has (Interface) " << *cit;
                    otherResourceInterfaces.push_back(*cit);
                }
            }
        }
    }

    LOG_INFO_S << "Check compatibility: " << instance << " <--> " << otherInstance << std::endl
        << "    used instances by first:        " << resourceUsedInstances << std::endl
        << "    used instances by second:       " << otherResourceUsedInstances << std::endl
        << "    available interfaces by first:  " << resourceInterfaces << std::endl
        << "    available interfaces by second: " << otherResourceInterfaces << std::endl;

    if(resourceInterfaces.empty() || otherResourceInterfaces.empty())
    {
        LOG_INFO_S << "'" << instance << "' or '" << otherInstance << "' has no interfaces -- returning empty candidate list";
        return CandidatesList();
    }

    CandidatesList candidates;
    // Identify how both resources can be matched -- i.e. which interfaces mapping are available
    // Currently greedy: picks the first match
    IRIList::const_iterator rit = resourceInterfaces.begin();
    for(; rit != resourceInterfaces.end(); ++rit)
    {
        IRI interface = *rit;
        IRI interfaceModel = getResourceModel(interface);

        LOG_INFO_S << "Checking resource: '" << interface << "' of '" << instance << "' on compatiblity";

        IRIList::const_iterator oit = otherResourceInterfaces.begin();
        for(; oit != otherResourceInterfaces.end(); ++oit)
        {
            IRI otherInterface = *oit;
            IRI otherInterfaceModel = getResourceModel(otherInterface);


            if( mpOntology->isRelatedTo(interfaceModel, OM::compatibleWith(), otherInterfaceModel) )
            {
                LOG_INFO_S << instance << " compatibleWith " << otherInstance << " via " << interface << " and " << otherInterface;
                usedInterfaces.push_back(interface);
                usedInterfaces.push_back(otherInterface);

                // One candidate for the matching
                candidates.push_back(usedInterfaces);
            }
        }
    }

    if(candidates.empty())
    {
        LOG_INFO_S << instance << " is not compatibleWith " << otherInstance << " via any interface";
    } else {
        LOG_INFO_S << "Candidates: " << candidates;
    }
    return candidates;
}

IRI OrganizationModel::getResourceModel(const IRI& instance) const
{
    try {
        return mpOntology->relatedInstance(instance, OM::modelledBy());
    } catch(const std::invalid_argument& e)
    {
        // no model means, this instance is a model by itself
        return instance;
    }
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
    IRIList interfaces;
    IRIList actors = mpOntology->allInstancesOf( OM::Actor() );
    {
        IRIList::const_iterator ait = actors.begin();
        for(; ait != actors.end(); ++ait)
        {
            // Identify the list of interfaces for 'instance'
            IRIList actorInterfaces = mpOntology->allRelatedInstances( *ait, OM::has());
            IRIList::const_iterator cit = actorInterfaces.begin();
            for(; cit != actorInterfaces.end(); ++cit)
            {
                if( mpOntology->isInstanceOf(*cit, OM::Interface()) )
                {
                    interfaces.push_back(*cit);
                }
            }
        }
    }

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

        if( ! mpOntology->isRelatedTo( getResourceModel( match[0] ), OM::compatibleWith(), getResourceModel( match[1] )) )
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
        // Validate by caching all interface that are used by previous connections
        InterfaceConnectionList usedInterfaceConnections;
        for(; cit != connectionList.end(); ++cit)
        {
            InterfaceConnection connection = *cit;

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

}
