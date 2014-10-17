#include "CAF.hpp"
#include <set>
#include <iostream>

void append(std::vector<char>& v, char c, size_t n)
{
        for(size_t i = 0; i < n; ++i)
        {
            v.push_back(c);
        }
}

int main()
{
    {
        std::vector<char> input;
        append(input, 'a', 1);
        append(input, 'b', 3);
        append(input, 'c', 2);
        append(input, 'p', 10);

        base::combinatorics::Combination<char> combinations( input, input.size(), base::combinatorics::MAX);
        size_t count = 0;
        do {
            std::vector<char> characterCombo = combinations.current();
            std::cout << std::string(characterCombo.begin(), characterCombo.end()) << std::endl;
            ++count;
        } while(combinations.next());
        std::cout << "Total number of combinations: " << count << std::endl;
    }

//    using namespace multiagent::caf;
//    std::vector<Interface> interfaces;
//    std::vector<Actor> actors;
//
//    uint8_t numberOfActorInstances = 10;
//    uint8_t numberOfActorInterfaces = 1;
//
//
//    // Initialize actors
//    for(uint8_t a = 'a'; a < 'a' + numberOfActorInstances; ++a)
//    {
//        actors.push_back(a);
//
//        // Initialize interfaces
//        for(uint8_t i = 0; i < numberOfActorInterfaces; ++i)
//        {
//            Interface interface(a,i);
//            interfaces.push_back(interface);
//        }
//    }
//
//
//    // Create links
//    using namespace base::combinatorics;
//    base::combinatorics::Combination<Interface> combinations(interfaces, 2);
//    std::vector<Link> links;
//    do {
//        std::vector<Interface> interfaceCombination = combinations.current();
//        links.push_back( Link(interfaceCombination[0], interfaceCombination[1] ) );
//    } while(combinations.next());
//
//    // Remove links that violate general constraints
//    std::vector<Link> validLinks;
//    // All links that connect the same actors
//    std::map<LinkGroupId, std::set<Link> > linkGroupMap;
//    std::set<LinkGroupId> availableLinkGroups;
//
//    std::map<Actor, std::set<Link> > actorLinkMap;
//    std::map<InterfaceId, std::set<Link> > interfaceLinkMap;
//
//    BOOST_FOREACH(Link link, links)
//    {
//        if(link.getFirstActor() == link.getSecondActor())
//        {
//            continue;
//        }
//        validLinks.push_back(link);
//
//        // Create link groups
//        linkGroupMap[link.getLinkGroupId()].insert(link);
//        availableLinkGroups.insert(link.getLinkGroupId());
//
//        // Create interface mapping
//        LOG_DEBUG_S << "Add interface (" << link.getFirstInterface().getId() << ")" << interfaceLinkMap[link.getFirstInterface().getId()].size();
//        LOG_DEBUG_S << "Add interface (" << link.getSecondInterface().getId() << ")";
//        interfaceLinkMap[link.getFirstInterface().getId()].insert(link);
//        interfaceLinkMap[link.getSecondInterface().getId()].insert(link);
//
//        // Create actor mapping
//        actorLinkMap[link.getFirstActor()].insert(link);
//        actorLinkMap[link.getSecondActor()].insert(link);
//    }   
//
//    LOG_DEBUG_S << "Valid links";
//    BOOST_FOREACH(Link link, validLinks)
//    {
//        LOG_DEBUG_S << link;
//    }
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
//    // Create seed, i.e. initialize combined actors for link count 0
//    std::set<CombinedActor> combinedActors;
//    BOOST_FOREACH(Link link, validLinks)
//    {
//        CombinedActor combinedActor(link, availableLinkGroups, &linkGroupMap, &interfaceLinkMap);
//        combinedActors.insert(combinedActor);
//    }
//    std::vector< std::set<CombinedActor> > allCombinedActors;
//    allCombinedActors.push_back(combinedActors);
//
//    uint32_t combinationCount = 0;
//    // Create valid combinations of links, i.e. valid actors ( limited to the number of actor available )
//    // Pick one link per link group
//    for(size_t linkCount = 1; linkCount < numberOfActorInstances; ++linkCount)
//    {
//        if(allCombinedActors.size() == linkCount)
//        {
//            // no need to recompute combined actors for given link count
//            continue;
//        } else {
//            // foreach each combined actor, pick a valid combination link group
//            // Pick a combined actor from current linkCount -1 
//
//            // Store combined actors for current link count
//            std::set<CombinedActor> combinedActors;
//
//            LOG_DEBUG("Compute for link count: %d", linkCount);
//            const std::set<CombinedActor>& combinedActorsOfSize = allCombinedActors[(linkCount-1) -1];
//            std::set<CombinedActor>::const_iterator ait = combinedActorsOfSize.begin();
//            for(; ait != combinedActorsOfSize.end(); ++ait)
//            {
//                std::vector<Link> whitelistLinks = ait->getWhitelistLinks();
//
////              //  LOG_DEBUG("Whitelist link: %d", whitelistLinks.size());
//                ////// Combine actor with another CombinedActor of size 1
//                std::vector<Link>::const_iterator cit = whitelistLinks.begin();
//                for(; cit != whitelistLinks.end(); ++cit)
//                {
//                    CombinedActor newActor = *ait;
//                    newActor.addLink(*cit);
//                    combinedActors.insert(newActor);
//                    combinationCount++;
//                }
//            }
//            allCombinedActors.push_back(combinedActors);
//
//            //LOG_DEBUG("New actors %d for link count %d", combinedActors.size(), linkCount);
//            //BOOST_FOREACH(CombinedActor actor, combinedActors)
//            //{
//            //    LOG_DEBUG_S << actor;
//            //}
//        }
//    }

    return 0;
}

