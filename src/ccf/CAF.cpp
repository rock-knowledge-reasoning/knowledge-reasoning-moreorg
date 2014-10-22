#include "CAF.hpp"
#include <set>
#include <iostream>
#include <math.h>

int main()
{
    using namespace multiagent::caf;
    size_t numberOfActorTypes;

    std::cout << "How many different actor types are being used:" << std::endl;
    std::cin >> numberOfActorTypes;
    std::vector<Actor> actors;

    char currentActorType = 'a';
    std::cout << "Please provide information about the actor types: " << std::endl;

    std::vector<Interface> interfaces;
    std::set<Interface> interfaceSet;
    for(int i = 0; i < numberOfActorTypes; ++i)
    { 
        std::cout << "The available number of items per type:" << std::endl;

        size_t numberPerActorType;
        std::cout << currentActorType << ": ";
        std::cin >> numberPerActorType;

        size_t maleEmiCount;
        std::cout << "The available number of EMI+ for this type:" << std::endl;
        std::cout << "Male EMI: " << std::endl;
        std::cin >> maleEmiCount;

        size_t femaleEmiCount;
        std::cout << "The available number of EMI- for this type:" << std::endl;
        std::cout << "Female EMI: " << std::endl;
        std::cin >> femaleEmiCount;

        uint8_t localActorId = 0;

        // Add interfaces per actor
        for(size_t i = 0; i < numberPerActorType; ++i)
        {
            Actor actor(currentActorType, localActorId++);
            actors.push_back(actor);

            // Initialize interfaces
            LocalInterfaceId localInterfaceId = 0;
            for(uint8_t i = 0; i < maleEmiCount; ++i)
            {
                Interface interface(actor,localInterfaceId++,'m');
                interfaces.push_back(interface);
                interfaceSet.insert(interface);
            }

            // Initialize interfaces
            for(uint8_t i = 0; i < femaleEmiCount; ++i)
            {
                Interface interface(actor, localInterfaceId++,'f');
                interfaces.push_back(interface);
                interfaceSet.insert(interface);
            }
        }
        ++currentActorType;

    }

    // Lets assume we have actor and interfaces that are all compatible
    uint8_t numberOfActorInstances = actors.size();

    // Create links
    using namespace base::combinatorics;
    base::combinatorics::Combination<Interface> combinations(interfaces, 2, base::combinatorics::EXACT);
    int count = 0;
    std::vector<Link> links;
    std::vector<Link> invalidLinks;
    do {
        std::vector<Interface> interfaceCombination = combinations.current();
        Link link(interfaceCombination[0], interfaceCombination[1] );
        if(interfaceCombination[0].getType() == interfaceCombination[1].getType())
        {
            invalidLinks.push_back(link);
        } else {
            links.push_back( link );
        }
    } while(combinations.next());


    // Remove links that violate general constraints
    std::vector<Link> validLinks;
    // All links that connect the same actors
    std::map<LinkGroup, std::set<Link> > linkGroupMap;
    std::set<LinkGroup> availableLinkGroups;

    std::map<Actor, std::set<Link> > actorLinkMap;
    std::map<Interface, std::set<Link> > interfaceLinkMap;

    BOOST_FOREACH(Link link, links)
    {
        if(link.getFirstActor() == link.getSecondActor())
        {
            invalidLinks.push_back(link);
            continue;
        }
        validLinks.push_back(link);

        // Create link groups
        linkGroupMap[link.getLinkGroup()].insert(link);
        availableLinkGroups.insert(link.getLinkGroup());

        // Create interface mapping
        interfaceLinkMap[link.getFirstInterface()].insert(link);
        interfaceLinkMap[link.getSecondInterface()].insert(link);

        // Create actor mapping
        actorLinkMap[link.getFirstActor()].insert(link);
        actorLinkMap[link.getSecondActor()].insert(link);
    }   

    std::cout << "# actors:           " << actors.size() << std::endl;
    std::cout << "# interfaces:       " << interfaces.size() << std::endl;
    std::cout << "# links (expected): "<< combinations.numberOfCombinations() << std::endl;
    std::cout << "# valid links:      " << validLinks.size() << std::endl;
    std::cout << "# invalid links:    " << invalidLinks.size() << std::endl;
    BOOST_FOREACH(Link link, validLinks)
    {
        LOG_DEBUG_S << link;
    }
    exit(0);

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
    size_t maxCount = std::min((size_t) numberOfActorInstances - 1, (size_t) 5);
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
                std::set<Link> whitelistLinks = ait->getWhitelistLinks();

//              //  LOG_DEBUG("Whitelist link: %d", whitelistLinks.size());
                ////// Combine actor with another CombinedActor of size 1
                std::set<Link>::const_iterator cit = whitelistLinks.begin();
                for(; cit != whitelistLinks.end(); ++cit)
                {
                    try {
                        CombinedActor newActor = *ait;
                        newActor.addLink(*cit);
                        combinedActors.insert(newActor);
                        combinationCount++;
                        LOG_DEBUG_S << newActor;
                    } catch(const std::runtime_error& e)
                    {
                        LOG_DEBUG_S << "Exception: " << e.what() << std::endl;
                    }
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

