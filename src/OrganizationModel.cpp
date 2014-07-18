#include "OrganizationModel.hpp"
#include <algorithm>
#include <boost/assign/list_of.hpp>
#include <boost/lambda/lambda.hpp>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Combinatorics.hpp>

using namespace owl_om::vocabulary;

namespace owl_om {

bool InterfaceConnection::operator<(const InterfaceConnection& other) const
{
    return begin < other.begin && end < other.end;

}

OrganizationModel::OrganizationModel()
    : mpOntology( new Ontology())
{}

OrganizationModel::OrganizationModel(const std::string& filename, bool runInference)
{
    mpOntology = Ontology::fromFile(filename);
    mpOntology->refresh();

    generateInterfaceCombinations();

//    if(runInference)
//    {
//        runInferenceEngine();
//    }
}

void OrganizationModel::createInstance(const IRI& instanceName, const IRI& klass, const IRI& model)
{
    mpOntology->instanceOf(instanceName, klass);
    mpOntology->relatedTo(instanceName, OM::modelledBy(), model);
}

void OrganizationModel::runInferenceEngine()
{
    try {
        IRIList actors = computeActorsFromRecombination();
        LOG_INFO_S << "Inferred actors: " << actors;
    } catch(const std::invalid_argument& e)
    {
        LOG_WARN_S << e.what();
    }

    IRIList actors = mpOntology->allInstancesOf( OM::ActorModel() );
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


// Provide list of available actor permutations
IRIList OrganizationModel::computeActorsFromRecombination()
{
    IRIList newActors;
    // identify all actors to create permutation ..
    // create permutation using the direct instances only -- composite ones will extend
    // 'CompositeActor'
    IRIList actors = mpOntology->allInstancesOf(OM::Actor(), true /* direct instances only */);
    if(actors.empty())
    {
        throw std::invalid_argument("OrganizationModel::computeActorsFromRecombination there are no actors to recombine");
    } else {
        LOG_DEBUG_S << "Number of actors for recombination: " << actors.size();
    }

    // remove all aliases
    actors = mpOntology->uniqueList(actors);
    IRIList combinators = actors;

    IRIList::const_iterator ait = actors.begin();
    for(; ait != actors.end(); ++ait)
    {
        IRIList rest = combinators;
        IRIList::iterator self = std::find(rest.begin(), rest.end(), *ait);
        if(self != rest.end())
        {
            rest.erase(self);
        }

        LOG_INFO_S << "Recombine '" << *ait << " with " << rest;
        IRIList recombinations = recombine(*ait, rest);
        newActors.insert(newActors.begin(), recombinations.begin(), recombinations.end());
    }

    return newActors;
}


IRIList OrganizationModel::recombine(const IRI& actor, const IRIList& otherActors)
{
    IRIList::const_iterator cit = otherActors.begin();
    IRIList recombinations;
    for(; cit != otherActors.end(); ++cit)
    {
        // Try reconfiguration
        CandidatesList interfaceMatchings = checkIfCompatibleNow(actor, *cit);
        if(interfaceMatchings.empty())
        {
            LOG_DEBUG_S << "Cannot join: '" << actor <<"' and '" << *cit << "' -- no interfaces available";
            continue;
        } else {
            LOG_DEBUG("Can join: '%s' and '%s' in %d different ways", actor.toString().c_str(), cit->toString().c_str(), interfaceMatchings.size());
        }

        CandidatesList::iterator mit = interfaceMatchings.begin();
        // Identifing different version depending on the interface matching
        int candidateId = 0;
        for(; mit != interfaceMatchings.end(); ++mit)
        {
            IRIList usedInterfaces = *mit;
    
            // TODO: link interfaces that are involved in this reconfiguration
            // and add actor dependant labelling of the edge to allow filtering
    
            // reconfiguration with of the same model is generally possible
            // so leave it here for the moment
            // TODO: FIX IRI recombination handling
            IRI actorFromRecombination = IRI::create( actor.toString() + "+" + cit->getFragment() );
            std::stringstream ss;
            ss << getResourceModel(actor) << "+" << getResourceModel(*cit).getFragment() << "[" << candidateId++ << "]";
            IRI actorClassFromRecombination = IRI::create(ss.str());
    
            mpOntology->subclassOf(OM::CompositeActor(), OM::Actor());
            mpOntology->instanceOf(actorClassFromRecombination, OM::CompositeActor());
            mpOntology->relatedTo(actorFromRecombination, OM::modelledBy(), actorClassFromRecombination);
    
            mpOntology->relatedTo(actorFromRecombination, OM::has(), actor);
            mpOntology->relatedTo(actorFromRecombination, OM::has(), *cit);
    
            mpOntology->relatedTo(actorClassFromRecombination, OM::dependsOn(), getResourceModel(actor));
            mpOntology->relatedTo(actorClassFromRecombination, OM::dependsOn(), getResourceModel(*cit));
    
            IRIList::iterator iit = usedInterfaces.begin();
            for(; iit != usedInterfaces.end(); ++iit)
            {
                mpOntology->relatedTo(actorClassFromRecombination, IRI::create("uses"), *iit);
            }
    
            LOG_INFO_S << "New actor: '" << actorFromRecombination << std::endl
                << "     new actor model:    " << actorClassFromRecombination << std::endl
                << "     involved actors:    " << actor << " and " << *cit << std::endl
                << "     recombination uses: " << usedInterfaces;
    
            // Update return list and use the current instance of the newly inferred actor type
            recombinations.push_back(actorFromRecombination);
    
            // Prepare further recombinations based on the new actor
            IRIList rest = otherActors;
            IRIList::iterator self = std::find(rest.begin(), rest.end(), *cit);
            rest.erase(self);
    
            if(!rest.empty())
            {
                // Recombine the newly found actor with remaining atomic actors
                IRIList addedActors = recombine(actorFromRecombination, rest);
                recombinations.insert(recombinations.begin(), addedActors.begin(), addedActors.end());
            }
        }
    }

    return recombinations;
}

IRI OrganizationModel::getResourceModel(const IRI& instance)
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
    // 2. use connections to compute combinations of order 1 to N-1, where N is the number of actors
    //    - N-1 since two actors can have only 1 connection
    //    

    IRIList interfaces = mpOntology->allInstancesOf( OM::Interface() );
    InterfaceConnectionList validConnections;

    if(interfaces.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough interfaces available, i.e. no more than 1");
    }

    // Get all basic actors to get number of actors and maximum number of connections
    // we have to account for
    IRIList actors = mpOntology->allInstancesOf( OM::ActorModel() );
    if(actors.size() < 2)
    {
        throw std::invalid_argument("owl_om::OrganizationModel::generateInterfaceCombination: not enough actors available, i.e. no more than 1");
    }

    Combination<IRI> interfaceCombinations(interfaces, 2, Combination<IRI>::EXACT);
    do {

        IRIList match = interfaceCombinations.current();
        LOG_WARN_S << "MATCH size: " << match.size() << " from " << interfaces.size();
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

        validConnections.push_back( InterfaceConnection(match[0], match[1]) );
    } while( interfaceCombinations.next() );


    size_t maximumNumberOfConnections =  actors.size() - 1;

    // Compute valid combinations, i.e.
    //    - each interface is only used once in a combination
    InterfaceCombinationList validCombinations; 
    Combination<InterfaceConnection> connections( validConnections, maximumNumberOfConnections, Combination<InterfaceConnection>::MAX);
    do {
        IRIList usedInterfaces;
        InterfaceConnectionList connectionList = connections.current();
        InterfaceConnectionList::const_iterator cit = connectionList.begin();
        bool valid = true;
        for(; cit != connectionList.end(); ++cit)
        {
            InterfaceConnection connection = *cit;

            IRIList::const_iterator uit = std::find_if(usedInterfaces.begin(), usedInterfaces.end(), boost::lambda::_1 == connection.begin || boost::lambda::_1 == connection.end );
            if(uit != usedInterfaces.end())
            {
                valid = false;
                break;
            }
        }
        if(valid)
        {
            validCombinations.push_back(connectionList);
        }
    } while( connections.next() );

    LOG_DEBUG_S << "Found '" << validCombinations.size() << "' valid combinations";
    return validCombinations;
}

}
