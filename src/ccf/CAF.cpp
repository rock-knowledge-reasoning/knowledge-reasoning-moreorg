#include <set>
#include <iostream>
#include <math.h>
#include <boost/foreach.hpp>
#include <base/Time.hpp>
#include <base-logging/Logging.hpp>
#include "CombinedActor.hpp"
#include "Scenario.hpp"

using namespace multiagent::ccf;

int main()
{
    Scenario scenario = Scenario::fromConsole();

    // Required input parameters
    std::vector<Link> validLinks = scenario.getValidLinks();

    // All links that connect the same actors
    std::map<LinkGroup, std::set<Link> > linkGroupMap = scenario.getLinkGroupMap();
    std::set<LinkGroup> availableLinkGroups = scenario.getAvailableLinkGroups();
    std::map<Actor, std::set<Link> > actorLinkMap = scenario.getActorLinkMap();
    std::map<Interface, std::set<Link> > interfaceLinkMap = scenario.getInterfaceLinkMap();

    uint8_t numberOfActorInstances = scenario.getNumberOfActors();
    // End required input parameters

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

