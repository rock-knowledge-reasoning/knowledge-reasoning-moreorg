#include "OrganizationModel.hpp"
#include <algorithm>
#include <boost/assign/list_of.hpp>

namespace owl_om {

std::map<OrganizationModel::Entities, IRI> OrganizationModel::EntitiesIRIs = boost::assign::map_list_of
    (OrganizationModel::ACTOR, "Actor")
    (OrganizationModel::INTERFACE, "Interface")
    (OrganizationModel::COMPATIBILITY, "Compatibility")
    (OrganizationModel::SERVICE, "Service");

std::map<OrganizationModel::Properties, IRI> OrganizationModel::PropertiesIRIs = boost::assign::map_list_of
    (OrganizationModel::DEPENDS_ON, "dependsOn")
    (OrganizationModel::HAS, "has")
    (OrganizationModel::PROVIDES, "provides")
    (OrganizationModel::USES, "uses");

void OrganizationModel::runInferenceEngine()
{
    std::vector<std::string> actors = mKnowledgeBase.allInstancesOf( EntitiesIRIs[ACTOR] );
    std::vector<std::string> services = mKnowledgeBase.allInstancesOf( EntitiesIRIs[SERVICE] );
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
                    mKnowledgeBase.relatedTo(*actorIt, "provides", *serviceIt);
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
    IRIList availableResources = mKnowledgeBase.allRelatedInstances(resourceProvider, PropertiesIRIs[HAS]);
    IRIList availableServices = mKnowledgeBase.allRelatedInstances(resourceProvider, PropertiesIRIs[PROVIDES] );
    IRIList requirements = mKnowledgeBase.allRelatedInstances(resourceRequirements, PropertiesIRIs[DEPENDS_ON]);

    LOG_DEBUG("check if resourceProvider '%s' fulfilly some of the '%d' requirements", resourceProvider.c_str(), requirements.size());
    IRIList::const_iterator cit = requirements.begin();
    for(; cit != requirements.end(); ++cit)
    {
        IRI requirementName = *cit;
        IRIList::iterator nit = availableResources.begin();
        bool dependencyFulfilled = false;
        for(; nit != availableResources.end(); ++nit)
        {
            IRI availableResource = *nit;
            if( mKnowledgeBase.isSameClass(requirementName, availableResource) )
            {
                LOG_INFO_S << "requirement " << requirementName << " fulfilled by '" << resourceProvider << "' using '"<< availableResource << "'";
                dependencyFulfilled = true;
                break;
            }
        }

        IRIList::iterator sit = std::find(availableServices.begin(), availableServices.end(), requirementName);
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
    {
        IRIList interfaces = mKnowledgeBase.allRelatedInstances(resource, PropertiesIRIs[HAS]);
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
        IRIList interfaces = mKnowledgeBase.allRelatedInstances(otherResource, PropertiesIRIs[HAS]);
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

    IRIList::const_iterator rit = resourceInterfaces.begin();
    IRIList::const_iterator oit = otherResourceInterfaces.begin();
    for(; rit != resourceInterfaces.end(); ++rit)
    {
        IRI interface = *rit;
        IRI interfaceType = mKnowledgeBase.typeOf(interface);

        for(; oit != otherResourceInterfaces.end(); ++oit)
        {
            IRI otherInterface = *oit;
            IRI otherInterfaceType = mKnowledgeBase.typeOf(otherInterface);

            if( mKnowledgeBase.assertAndAddRelation(interface, "compatibleWith", otherInterface) )
            {
                LOG_INFO_S << resource << " compatibleWith " << otherResource << " via " << interface << " and " << otherInterface;
                return true;
            }

            //if( mKnowledgeBase.isSubclassOf( mKnowledgeBase.intersectionOf(interfaceType, otherInterfaceType), EntitiesIRIs[COMPATIBILITY]) )
            //{
            //    LOG_INFO_S << resource << " compatibleWith " << otherResource << " via " << interface << " and " << otherInterface;
            //    return true;
            //}
        }
    }

    return false;
}

IRIList OrganizationModel::checkIfCompatibleNow(const std::string& instance, const std::string& otherInstance)
{
    IRIList usedInterfaces;

    IRIList resourceInterfaces;
    IRIList otherResourceInterfaces;

    IRIList resourceUsedInstances = mKnowledgeBase.allRelatedInstances(instance, PropertiesIRIs[USES]);
    IRIList otherResourceUsedInstances = mKnowledgeBase.allRelatedInstances(otherInstance, PropertiesIRIs[USES]);

    {
        IRIList interfaces = mKnowledgeBase.allRelatedInstances(instance, PropertiesIRIs[HAS]);
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
        IRIList interfaces = mKnowledgeBase.allRelatedInstances(otherInstance, PropertiesIRIs[HAS]);
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

    IRIList::const_iterator rit = resourceInterfaces.begin();
    IRIList::const_iterator oit = otherResourceInterfaces.begin();
    for(; rit != resourceInterfaces.end(); ++rit)
    {
        IRI interface = *rit;
        IRI interfaceType = mKnowledgeBase.typeOf(interface);

        for(; oit != otherResourceInterfaces.end(); ++oit)
        {
            IRI otherInterface = *oit;
            IRI otherInterfaceType = mKnowledgeBase.typeOf(otherInterface);

            if( mKnowledgeBase.isSubclassOf( mKnowledgeBase.intersectionOf(interfaceType, otherInterfaceType), EntitiesIRIs[COMPATIBILITY]) )
            {
                LOG_INFO_S << instance << " compatibleWith " << otherInstance << " via " << interface << " and " << otherInterface;
                usedInterfaces.push_back(interface);
                usedInterfaces.push_back(otherInterface);
                return usedInterfaces;
            }
        }
    }

    return usedInterfaces;
}


// Provide list of available actor permutations
IRIList OrganizationModel::computeActorsFromRecombination()
{
    IRIList newActors;
    // identify all actor to create permutation ..
    // create permutation
    IRIList actors = mKnowledgeBase.allInstancesOf(EntitiesIRIs[ACTOR]);
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
        IRIList usedInterfaces = checkIfCompatibleNow(actor, *cit);
        if(usedInterfaces.empty())
        {
            LOG_INFO("Cannot join: '%s' and '%s' -- no interfaces available", actor.c_str(), cit->c_str());
            continue;
        }

        // TODO: link interfaces that are involved in this reconfiguration
        // and add actor dependant labelling of the edge to allow filtering

        // reconfiguration with of the same model is generally possible
        // so leave it here for the moment
        IRI actorFromRecombination = actor + "-" + *cit;
        mKnowledgeBase.subclassOf(actorFromRecombination, EntitiesIRIs[ACTOR]);
        mKnowledgeBase.relatedTo(actorFromRecombination, PropertiesIRIs[HAS], actor);
        mKnowledgeBase.relatedTo(actorFromRecombination, PropertiesIRIs[HAS], *cit);

        std::vector<std::string>::iterator iit = usedInterfaces.begin();
        for(; iit != usedInterfaces.end(); ++iit)
        {
            LOG_INFO("add new actor: '%s' uses '%s'", actorFromRecombination.c_str(), iit->c_str());
            mKnowledgeBase.relatedTo(actorFromRecombination, "uses", *iit);
        }

        LOG_INFO("new actor: '%s' --> '%s' combined with '%s'", actorFromRecombination.c_str(), actor.c_str(), cit->c_str());

        // Update return list
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

    return recombinations;
}

}
