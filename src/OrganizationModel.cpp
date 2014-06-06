#include "OrganizationModel.hpp"
#include <algorithm>
#include <boost/assign/list_of.hpp>

namespace owl_om {

std::map<OrganizationModel::Entities, IRI> OrganizationModel::EntitiesIRIs = boost::assign::map_list_of
    (OrganizationModel::ACTOR, "Actor")
    (OrganizationModel::ACTOR_MODEL, "ActorModel")
    (OrganizationModel::COMPOSITE_ACTOR, "CompositeActor")
    (OrganizationModel::INTERFACE, "Interface")
    (OrganizationModel::SERVICE, "Service")
    (OrganizationModel::SERVICE_MODEL, "ServiceModel");

std::map<OrganizationModel::Properties, IRI> OrganizationModel::PropertiesIRIs = boost::assign::map_list_of
    (OrganizationModel::DEPENDS_ON, "dependsOn")
    (OrganizationModel::HAS, "has")
    (OrganizationModel::PROVIDES, "provides")
    (OrganizationModel::USES, "uses")
    (OrganizationModel::COMPATIBLE_WITH, "compatibleWith")
    (OrganizationModel::MODELED_BY, "modeledBy");


void OrganizationModel::createInstance(const IRI& instanceName, const IRI& klass, const IRI& model)
{
    mKnowledgeBase.instanceOf(instanceName, klass);
    mKnowledgeBase.relatedTo(instanceName, PropertiesIRIs[MODELED_BY], model);
}

void OrganizationModel::runInferenceEngine()
{
    std::vector<std::string> actors = mKnowledgeBase.allInstancesOf( EntitiesIRIs[ACTOR_MODEL] );
    std::vector<std::string> services = mKnowledgeBase.allInstancesOf( EntitiesIRIs[SERVICE_MODEL] );
    std::vector<std::string>::const_iterator actorIt = actors.begin();

    LOG_INFO_S << "Run inference engine: # of actors: '" << actors.size();

    bool updated = true;
    while(updated)
    {
        updated = false;
        for(; actorIt != actors.end(); ++actorIt)
        {
            std::vector<std::string>::const_iterator serviceIt = services.begin();
            for(; serviceIt != services.end(); ++serviceIt)
            {
                LOG_INFO("inference: '%s' checkIfFulfills? '%s'", actorIt->c_str(), serviceIt->c_str());
                if( checkIfFulfills(*actorIt, *serviceIt) )
                {
                    LOG_INFO("inference: '%s' provides '%s'", actorIt->c_str(), serviceIt->c_str());
                    mKnowledgeBase.relatedTo(*actorIt, PropertiesIRIs[PROVIDES], *serviceIt);
                    updated = true;
                } else {
                    LOG_INFO("inference: '%s' does not provide '%s'", actorIt->c_str(), serviceIt->c_str());
                }
            }
        }
    }
}

bool OrganizationModel::checkIfFulfills(const IRI& resourceProvider, const IRI& resourceRequirements)
{
    IRI resourceProviderModel = getResourceModel(resourceProvider);

    IRIList availableResources = mKnowledgeBase.allRelatedInstances(resourceProviderModel, PropertiesIRIs[HAS]);
    IRIList availableServices = mKnowledgeBase.allRelatedInstances(resourceProviderModel, PropertiesIRIs[PROVIDES]);

    LOG_DEBUG("Get requirements for '%s'", resourceRequirements.c_str());
    IRI requirementModel = getResourceModel(resourceRequirements);
    IRIList requirements = mKnowledgeBase.allRelatedInstances( requirementModel, PropertiesIRIs[DEPENDS_ON]);

    LOG_DEBUG("check if resourceProvider [%s] '%s' fulfills some of the '%d' requirements", resourceProviderModel.c_str(), resourceProvider.c_str(), requirements.size());

    IRIList::const_iterator cit = requirements.begin();
    for(; cit != requirements.end(); ++cit)
    {
        IRI requirementName = *cit;
        if(availableResources.empty())
        {
            LOG_DEBUG_S << "no available resources on '" << resourceProvider << "'";
        }
        IRIList::iterator nit = availableResources.begin();
        bool dependencyFulfilled = false;
        for(; nit != availableResources.end(); ++nit)
        {
            IRI availableResource = *nit;
            if( isSameResourceModel(requirementName, availableResource) )
            {
                LOG_INFO_S << "requirement " << requirementName << " fulfilled by '" << resourceProvider << "' using '"<< availableResource << "'";
                dependencyFulfilled = true;
                break;
            } else {
                LOG_INFO_S << "requirement " << requirementName << " not fulfilled using '"<< availableResource << "'";
            }
        }

        IRIList::iterator sit = std::find(availableServices.begin(), availableServices.end(), getResourceModel(requirementName));
        if(sit != availableServices.end())
        {
            IRI availableService = *sit;
            LOG_INFO_S << "requirement " << requirementName << " fulfilled by service " << availableService;
            dependencyFulfilled = true;
        }

        if(!dependencyFulfilled)
        {
            LOG_INFO_S << "requirement " << requirementName << " cannot be fulfilled by " << resourceProvider;
            return false;
        }
    }

    return true;
}

bool OrganizationModel::checkIfCompatible(const IRI& resource, const IRI& otherResource)
{
    IRIList resourceInterfaces;
    IRIList otherResourceInterfaces;

    // First extract available interfaces from the associated models
    {
        IRI resourceModel = getResourceModel(resource);
        IRIList interfaces = mKnowledgeBase.allRelatedInstances(resourceModel, PropertiesIRIs[HAS]);
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mKnowledgeBase.isInstanceOf(*cit, EntitiesIRIs[INTERFACE]) )
            {
                LOG_INFO_S << resource << " has (Interface) " << *cit;
                resourceInterfaces.push_back(*cit);
            }
        }
    }
    {
        IRI resourceModel = getResourceModel(otherResource);
        IRIList interfaces = mKnowledgeBase.allRelatedInstances(resourceModel, PropertiesIRIs[HAS]);
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mKnowledgeBase.isInstanceOf(*cit, EntitiesIRIs[INTERFACE]) )
            {
                LOG_INFO_S << otherResource << " has (Interface) " << *cit;
                otherResourceInterfaces.push_back(*cit);
            }
        }
    }

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

            if( mKnowledgeBase.isRelatedTo(interfaceModel, PropertiesIRIs[COMPATIBLE_WITH], otherInterfaceModel) )
            {
                LOG_INFO_S << resource << " compatibleWith " << otherResource << " via " << interface << "[" << interfaceModel << "] and " << otherInterface << "[" << otherInterfaceModel << "]";
                return true;
            } else {
                LOG_INFO_S << resource << " not compatibleWith " << otherResource << " via " << interface << "[" << interfaceModel << "] and " << otherInterface << "[" << otherInterfaceModel << "]";
            }
        }
    }

    LOG_INFO_S << resource << " is not compatibleWith " << otherResource << " via any interface";
    return false;
}

CandidatesList OrganizationModel::checkIfCompatibleNow(const std::string& instance, const std::string& otherInstance)
{
    IRIList usedInterfaces;

    IRIList resourceInterfaces;
    IRIList otherResourceInterfaces;

    IRIList resourceUsedInstances = mKnowledgeBase.allRelatedInstances(instance, PropertiesIRIs[USES]);
    IRIList otherResourceUsedInstances = mKnowledgeBase.allRelatedInstances(otherInstance, PropertiesIRIs[USES]);

    {
        // Identify the list of unused interfaces for 'instance'
        IRIList interfaces = mKnowledgeBase.allRelatedInstances( getResourceModel(instance), PropertiesIRIs[HAS]);
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mKnowledgeBase.isInstanceOf(*cit, EntitiesIRIs[INTERFACE]) )
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
        IRIList interfaces = mKnowledgeBase.allRelatedInstances( getResourceModel(otherInstance), PropertiesIRIs[HAS]);
        IRIList::const_iterator cit = interfaces.begin();
        for(; cit != interfaces.end(); ++cit)
        {
            if( mKnowledgeBase.isInstanceOf(*cit, EntitiesIRIs[INTERFACE]) )
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


            if( mKnowledgeBase.isRelatedTo(interfaceModel, PropertiesIRIs[COMPATIBLE_WITH], otherInterfaceModel) )
            {
                LOG_INFO_S << instance << " compatibleWith " << otherInstance << " via " << interface << " and " << otherInterface;
                usedInterfaces.push_back(interface);
                usedInterfaces.push_back(otherInterface);

                // One candidate for the matching
                candidates.push_back(usedInterfaces);
            } else {
                LOG_INFO_S << instance << " not compatibleWith " << otherInstance << " via " << interface << " and " << otherInterface;
            }
        }
    }

    if(candidates.empty())
    {
        LOG_INFO_S << instance << " is not compatibleWith " << otherInstance << " via any interface";
    } else {
        LOG_INFO_S << "Found number of candidates: " << candidates.size();
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
    IRIList actors = mKnowledgeBase.allInstancesOf(EntitiesIRIs[ACTOR], true /* direct instances only */);
    if(actors.empty())
    {
        throw std::invalid_argument("OrganizationModel::computeActorsFromRecombination there are no actors to recombine");
    } else {
        LOG_DEBUG_S << "Number of actors for recombination: " << actors.size();
    }

    // remove all aliases
    actors = mKnowledgeBase.uniqueList(actors);
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

        IRIList recombinations = recombine(*ait, rest);
        newActors.insert(newActors.begin(), recombinations.begin(), recombinations.end());
    }

    return newActors;
}


IRIList OrganizationModel::recombine(const IRI& actor, const IRIList& otherActors)
{
    LOG_INFO_S << "Recombine actor '" << actor << " with #" << otherActors.size() << " other actors";
    IRIList::const_iterator cit = otherActors.begin();
    IRIList recombinations;
    for(; cit != otherActors.end(); ++cit)
    {
        // Try reconfiguration
        CandidatesList interfaceMatchings = checkIfCompatibleNow(actor, *cit);
        if(interfaceMatchings.empty())
        {
            LOG_INFO("Cannot join: '%s' and '%s' -- no interfaces available", actor.c_str(), cit->c_str());
            continue;
        } else {
            LOG_INFO("Can join: '%s' and '%s' in %d different ways", actor.c_str(), cit->c_str(), interfaceMatchings.size());
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
            IRI actorFromRecombination = actor + "+" + *cit;
            std::stringstream ss;
            ss << getResourceModel(actor) << "+" << getResourceModel(*cit) << "[" << candidateId++ << "]";
            IRI actorClassFromRecombination = ss.str();
    
            mKnowledgeBase.subclassOf(EntitiesIRIs[COMPOSITE_ACTOR], EntitiesIRIs[ACTOR]);
            mKnowledgeBase.instanceOf(actorClassFromRecombination, EntitiesIRIs[COMPOSITE_ACTOR]);
            mKnowledgeBase.relatedTo(actorFromRecombination, PropertiesIRIs[MODELED_BY], actorClassFromRecombination);
    
            mKnowledgeBase.relatedTo(actorFromRecombination, PropertiesIRIs[HAS], actor);
            mKnowledgeBase.relatedTo(actorFromRecombination, PropertiesIRIs[HAS], *cit);
    
            mKnowledgeBase.relatedTo(actorClassFromRecombination, PropertiesIRIs[DEPENDS_ON], getResourceModel(actor));
            mKnowledgeBase.relatedTo(actorClassFromRecombination, PropertiesIRIs[DEPENDS_ON], getResourceModel(*cit));
    
            std::vector<std::string>::iterator iit = usedInterfaces.begin();
            for(; iit != usedInterfaces.end(); ++iit)
            {
                LOG_INFO("add new actor class: '%s' uses '%s'", actorClassFromRecombination.c_str(), iit->c_str());
                mKnowledgeBase.relatedTo(actorClassFromRecombination, "uses", *iit);
            }
    
            LOG_INFO("new actor: '%s' of class '%s' --> '%s' combined with '%s'", actorFromRecombination.c_str(), actorClassFromRecombination.c_str(), actor.c_str(), cit->c_str());
    
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
        return mKnowledgeBase.relatedInstance(instance, PropertiesIRIs[MODELED_BY]);
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

}
