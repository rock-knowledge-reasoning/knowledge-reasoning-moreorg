#include "CAF.hpp"
#include <set>
#include <iostream>
#include <math.h>

void append(std::vector<char>& v, char c, size_t n)
{
        for(size_t i = 0; i < n; ++i)
        {
            v.push_back(c);
        }
}

int main()
{
    using namespace multiagent::caf;
    std::vector<Interface> interfaces;
    std::vector<Actor> actors;

    // Lets assume we have actor and interfaces that are all compatible
    uint8_t numberOfActorInstances = 4;
    uint8_t numberOfActorInterfaces = 3;

    // Initialize actors
    for(uint8_t a = 'a'; a < 'a' + numberOfActorInstances; ++a)
    {
        actors.push_back(a);

        // Initialize interfaces
        for(uint8_t i = 0; i < numberOfActorInterfaces; ++i)
        {
            Interface interface(a,i);
            interfaces.push_back(interface);
        }
    }


    // Create links
    using namespace base::combinatorics;
    base::combinatorics::Combination<Interface> combinations(interfaces, 2);
    std::vector<Link> links;
    do {
        std::vector<Interface> interfaceCombination = combinations.current();
        links.push_back( Link(interfaceCombination[0], interfaceCombination[1] ) );
    } while(combinations.next());

    // Remove links that violate general constraints
    std::vector<Link> validLinks;
    // All links that connect the same actors
    std::map<LinkGroupId, std::set<Link> > linkGroupMap;
    std::set<LinkGroupId> availableLinkGroups;

    std::map<Actor, std::set<Link> > actorLinkMap;
    std::map<InterfaceId, std::set<Link> > interfaceLinkMap;

    BOOST_FOREACH(Link link, links)
    {
        if(link.getFirstActor() == link.getSecondActor())
        {
            continue;
        }
        validLinks.push_back(link);

        // Create link groups
        linkGroupMap[link.getLinkGroupId()].insert(link);
        availableLinkGroups.insert(link.getLinkGroupId());

        // Create interface mapping
        LOG_DEBUG_S << "Add interface (" << link.getFirstInterface().getId() << ")" << interfaceLinkMap[link.getFirstInterface().getId()].size();
        LOG_DEBUG_S << "Add interface (" << link.getSecondInterface().getId() << ")";
        interfaceLinkMap[link.getFirstInterface().getId()].insert(link);
        interfaceLinkMap[link.getSecondInterface().getId()].insert(link);

        // Create actor mapping
        actorLinkMap[link.getFirstActor()].insert(link);
        actorLinkMap[link.getSecondActor()].insert(link);
    }   

    std::cout << "# valid links: " << validLinks.size() << std::endl;
    BOOST_FOREACH(Link link, validLinks)
    {
        LOG_DEBUG_S << link;
    }

//    std::cout << "# actor combination up to size (MAX would be: " << actors.size() << ")" << std::endl;
//    size_t maxActorCombination;
//    std::in >> 10;
//
//    // Try for all combinations
//    std::vector<std::string> validActorCombinations;
//    {
//        Combination<Actor> actorCombinations(actors, actors.size() - 1, MAX);
//        do {
//            std::vector<Actor> actorCombination = actorCombinations.current();
//            std::sort(actorCombination.begin(), actorCombination.end());
//            std::string name(actorCombination.begin(), actorCombination.end());
//            validActorCombinations.push_back(name);
//        } while(actorCombinations.next());
//    }
//
//    //LOG_DEBUG("ActorCombinations: %d", validActorCombinations.size());
//    //std::map< std::set
//    //BOOST_FOREACH(std::string actorCombination, validActorCombinations)
//    //{
//    //}
//
//
    // Create seed, i.e. initialize combined actors for link count 0
    std::set<CombinedActor> combinedActors;
    BOOST_FOREACH(Link link, validLinks)
    {
        CombinedActor combinedActor(link, availableLinkGroups, &linkGroupMap, &interfaceLinkMap);
        combinedActors.insert(combinedActor);
    }

    std::vector< std::set<CombinedActor> > allCombinedActors;
    allCombinedActors.push_back(combinedActors);

    uint32_t combinationCount = 0;
    // Create valid combinations of links, i.e. valid actors ( limited to the number of actor available )
    // Pick one link per link group
    size_t maxCount = std::min((size_t) numberOfActorInstances - 1, (size_t) 10);
    std::cout << "Computing combinations per link count up to " << maxCount << std::endl;
    for(size_t linkCount = 1; linkCount <= maxCount; ++linkCount)
    {
        if(allCombinedActors.size() == linkCount)
        {
            std::cout << "Combined same as linkCount" << std::endl;
            // no need to recompute combined actors for given link count
            continue;
        } else {
            // foreach each combined actor, pick a valid combination link group
            // Pick a combined actor from current linkCount -1 

            // Store combined actors for current link count
            std::set<CombinedActor> combinedActors;

            std::cout << "Link count: " << linkCount << std::endl;

            base::Time start = base::Time::now();

            const std::set<CombinedActor>& combinedActorsOfSize = allCombinedActors[(linkCount-1) -1];
            std::set<CombinedActor>::const_iterator ait = combinedActorsOfSize.begin();
            for(; ait != combinedActorsOfSize.end(); ++ait)
            {
                std::vector<Link> whitelistLinks = ait->getWhitelistLinks();

//              //  LOG_DEBUG("Whitelist link: %d", whitelistLinks.size());
                ////// Combine actor with another CombinedActor of size 1
                std::vector<Link>::const_iterator cit = whitelistLinks.begin();
                for(; cit != whitelistLinks.end(); ++cit)
                {
                    CombinedActor newActor = *ait;
                    newActor.addLink(*cit);
                    combinedActors.insert(newActor);
                    combinationCount++;
                    std::cout << newActor;
                }
            }
            allCombinedActors.push_back(combinedActors);

            base::Time stop = base::Time::now();
            std::cout << "New actors " << combinedActors.size() << " for link count " << linkCount << std::endl;
            std::cout << "Computing time: " << (stop - start).toSeconds() << " seconds" << std::endl;
            //BOOST_FOREACH(CombinedActor actor, combinedActors)
            //{
            //    LOG_DEBUG_S << actor;
            //}
        }
    }

    return 0;
}

