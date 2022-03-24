#include "Scenario.hpp"
#include "../ModelPool.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <numeric/Combinatorics.hpp>
#include <numeric/LimitedCombination.hpp>

using namespace numeric;

namespace multiagent {
namespace ccf {

Scenario::Scenario()
{
    mInterfaceCompatibilityTypes.push_back('m');
    mInterfaceCompatibilityTypes.push_back('f');
}

Scenario Scenario::fromConsole()
{
    Scenario scenario;

    size_t numberOfActorTypes;
    std::cout << "How many different actor types are being used:" << std::endl;
    std::cin >> numberOfActorTypes;

    char currentActorType = 'a';
    std::cout << "Please provide information about the actor types: "
              << std::endl;

    for(size_t i = 0; i < numberOfActorTypes; ++i)
    {
        std::cout << "The available number of items per type:" << std::endl;

        size_t numberPerActorType;
        std::cout << currentActorType << ": ";
        std::cin >> numberPerActorType;

        ActorDescription actorDescription;

        size_t maleEmiCount;
        std::cout << "The available number of EMI+ for this type:" << std::endl;
        std::cout << "Male EMI: " << std::endl;
        std::cin >> maleEmiCount;
        actorDescription.setInterfaceCount('m', maleEmiCount);

        size_t femaleEmiCount;
        std::cout << "The available number of EMI- for this type:" << std::endl;
        std::cout << "Female EMI: " << std::endl;
        std::cin >> femaleEmiCount;
        actorDescription.setInterfaceCount('f', femaleEmiCount);
        scenario.mActorTypes.push_back(currentActorType);

        uint8_t localActorId = 0;
        // Add interfaces per actor
        for(size_t i = 0; i < numberPerActorType; ++i)
        {
            Actor actor(currentActorType, localActorId++);
            scenario.mActors[actor] = actorDescription;
        }

        scenario.setModelCount(currentActorType, numberPerActorType);
        ++currentActorType;
    }

    return scenario;
}

Scenario Scenario::fromFile(const std::string& filename)
{
    Scenario spec;
    std::ifstream specFile(filename);
    if(specFile.is_open())
    {
        std::string line;
        std::string delimiter = " ";
        size_t actorType = 0;
        while(getline(specFile, line))
        {
            // if line start with # then this is a comment
            if(line.find("#") == 0 || line.empty())
            {
                continue;
            }

            // column is
            // <# of instances> <# of male interfaces> <# of female interfaces>
            boost::char_separator<char> sep(" ");
            boost::tokenizer<boost::char_separator<char>> tokens(line, sep);

            std::vector<std::string> columns(tokens.begin(), tokens.end());
            if(columns.size() != 3)
            {
                std::invalid_argument(
                    "moreorg::ccf::Scenario: invalid column: '" + line +
                    "' in spec");
            }

            size_t numberOfInstances =
                boost::lexical_cast<size_t>(columns.at(0));
            size_t numberOfMaleInterfaces =
                boost::lexical_cast<size_t>(columns.at(1));
            size_t numberOfFemaleInterfaces =
                boost::lexical_cast<size_t>(columns.at(2));

            ActorDescription actorDescription;
            actorDescription.setInterfaceCount('m', numberOfMaleInterfaces);
            actorDescription.setInterfaceCount('f', numberOfFemaleInterfaces);

            char actorTypeId = 'a' + static_cast<char>(actorType);
            spec.mActorTypes.push_back(actorTypeId);

            for(size_t i = 0; i < numberOfInstances; ++i)
            {
                Actor actor(actorTypeId, i);
                spec.mActors[actor] = actorDescription;
            }
            spec.setModelCount(actorTypeId, numberOfInstances);

            ++actorType;
        }
        specFile.close();
    } else
    {
        throw std::runtime_error(
            "moreorg::ccf::Scenario: failed to load spec from: " + filename);
    }

    return spec;
}

void Scenario::compute(size_t maxCoalitionSize)
{
    mMaxCoalitionSize = maxCoalitionSize;

    std::map<Actor, ActorDescription>::const_iterator ait = mActors.begin();
    for(; ait != mActors.end(); ++ait)
    {
        Actor actor = ait->first;
        ActorDescription actorDescription = ait->second;

        LocalInterfaceId localInterfaceId = 0;

        // Collect all interfaces (actor, localinterface, interface type)
        // Add interfaces per actor
        std::vector<CompatibilityType>::const_iterator iit =
            mInterfaceCompatibilityTypes.begin();
        for(; iit != mInterfaceCompatibilityTypes.end(); ++iit)
        {
            // Initialize interfaces
            CompatibilityType interfaceType = *iit;
            for(uint8_t i = 0;
                i < actorDescription.getInterfaceCount(interfaceType);
                ++i)
            {
                Interface interface(actor, localInterfaceId++, interfaceType);
                mInterfaces.push_back(interface);
            }
        }

        mActorList.push_back(actor);
    }

    // Compute the maximum number of actor combinations
    Combination<Actor> combination(mActorList, mMaxCoalitionSize, MAX);
    mNumberOfActorCombinations = combination.numberOfCombinations();

    createLinks();
}

void Scenario::createLinks()
{
    std::vector<Link> links;

    // Using combination of links, automatically accounts for an undirected
    // link graph
    Combination<Interface> combinations(mInterfaces, 2, numeric::EXACT);

    // Compute all invalid and possibly(!) valid links
    do
    {
        std::vector<Interface> interfaceCombination = combinations.current();
        Link link(interfaceCombination[0], interfaceCombination[1]);
        // Same type is not compatible
        if(interfaceCombination[0].getCompatibilityType() ==
           interfaceCombination[1].getCompatibilityType())
        {
            mInvalidLinks.push_back(link);
            mInvalidLinkTypes.insert(link.getType());
        } else
        {
            links.push_back(link);
        }
    } while(combinations.next());

    // Check possibly valid links and
    // consider only links as valid when they do not
    // violate general constraints, e.g., disallowing self-referencing
    for(const Link& link : links)
    {
        if(link.getFirstActor() == link.getSecondActor())
        {
            mInvalidLinks.push_back(link);
            mInvalidLinkTypes.insert(link.getType());
            continue;
        }

        // Valid link found
        mValidLinks.push_back(link);
        mValidLinkTypes[link.getType()]++;

        // Create link groups, i.e. map pair of connecting agents to the link
        mLinkGroupMap[link.getGroup()].insert(link);
        // cache link groups, i.e. pairs of linked agents
        mAvailableLinkGroups.insert(link.getGroup());

        // Create interface mapping: map interface to the link it is involved
        mInterfaceLinkMap[link.getFirstInterface()].insert(link);
        mInterfaceLinkMap[link.getSecondInterface()].insert(link);

        // Create actor mapping: map actor to the number of links
        mActorLinkMap[link.getFirstActor()].insert(link);
        mActorLinkMap[link.getSecondActor()].insert(link);
    }

    moreorg::ModelPool::Set agentSpaceModelPools;
    std::set<std::set<Link>> linkSpaceModelPools;

    int count = 0;
    numeric::LimitedCombination<char> linkCombinations(mModelPool,
                                                       mMaxCoalitionSize,
                                                       numeric::MAX);
    do
    {
        ++count;
        std::vector<char> agentTypes = linkCombinations.current();
        if(agentTypes.size() == 1)
        {
            continue;
        }

        // Create a representative actor combination for this type
        std::vector<Actor> actors = mActorList;
        std::vector<Actor> representativeActor;
        for(char agentType : agentTypes)
        {
            std::vector<Actor>::iterator it =
                std::find_if(actors.begin(),
                             actors.end(),
                             [agentType](const Actor& actor) {
                                 return actor.getType() == agentType;
                             });
            representativeActor.push_back(*it);
            actors.erase(it);
        }

        // Find all feasible combinations for this type in link space
        //   std::cout << "Collect: " << count << std::endl;
        size_t coalitionSize =
            std::min(mMaxCoalitionSize, representativeActor.size());
        Combination<Actor> actorCombination(representativeActor,
                                            coalitionSize,
                                            EXACT);
        do
        {
            std::vector<Actor> actors = actorCombination.current();
            //        std::cout << "Actors: " << actors.size() << std::endl;
            collectCombinations(actors,
                                actors,
                                std::set<Link>(),
                                agentSpaceModelPools,
                                linkSpaceModelPools);
        } while(actorCombination.next());
    } while(linkCombinations.next());

    mNumberOfActorTypesTheoreticalBound = count;
    mNumberOfActorTypesAgentSpace =
        agentSpaceModelPools.size() + mActorTypes.size();
    mNumberOfActorTypesLinkSpace =
        linkSpaceModelPools.size() + mActorTypes.size();

    std::cout << "Max: " << mMaxCoalitionSize << std::endl;
    std::cout << "ModelPool: Agents " << agentSpaceModelPools.size()
              << std::endl
              << moreorg::ModelPool::toString(agentSpaceModelPools)
              << std::endl;
    // std::cout << "ModelPool: Link " << linkSpaceModelPools << std::endl;
}

void Scenario::collectCombinations(
    const std::vector<Actor>& allowedActors,
    std::vector<Actor> actors,
    std::set<Link> links,
    moreorg::ModelPool::Set& agentSpaceModelPools,
    std::set<std::set<Link>>& linkSpaceSets)
{
    if(actors.size() == 1)
    {
        actors.clear();

        std::set<Actor> actors;
        for(const Link& l : links)
        {
            actors.insert(l.getFirstActor());
            actors.insert(l.getSecondActor());
        }

        if(actors.size() == links.size() + 1)
        {
            moreorg::ModelPool agentSpaceAgentType;
            moreorg::ModelPool linkSpaceAgentType;
            for(Actor actor : actors)
            {
                std::string id;
                {
                    id += (char)actor.getType();
                    agentSpaceAgentType["http://actor-type#" + id] += 1;
                }
            }
            agentSpaceModelPools.insert(agentSpaceAgentType);
            linkSpaceSets.insert(links);
        }
    } else
    {
        std::set<Link> actorLinks = mActorLinkMap[actors.back()];
        std::set<Link> allowedLinks;
        for(const Link& l : actorLinks)
        {
            Actor a0 = l.getFirstActor();
            Actor a1 = l.getSecondActor();

            if(actors.end() !=
               std::find(allowedActors.begin(), allowedActors.end(), a0))
            {
                if(actors.end() !=
                   std::find(allowedActors.begin(), allowedActors.end(), a1))
                {
                    allowedLinks.insert(l);
                }
            }
        }
        actors.pop_back();

        // std::cout << "ActorLinks: " << allowedLinks.size() << std::endl;
        // for(const Link& l : allowedLinks)
        //{
        //    std::cout << "    " << l << std::endl;
        //}

        for(const Link& link : allowedLinks)
        {
            std::set<Link> addedLink = links;
            addedLink.insert(link);

            collectCombinations(allowedActors,
                                actors,
                                addedLink,
                                agentSpaceModelPools,
                                linkSpaceSets);
        }
    }
}

std::vector<LinkType> Scenario::getValidLinkTypeList() const
{
    std::map<LinkType, size_t>::const_iterator cit = mValidLinkTypes.begin();
    std::vector<LinkType> linkTypes;
    for(; cit != mValidLinkTypes.end(); ++cit)
    {
        linkTypes.push_back(cit->first);
    }
    return linkTypes;
}

std::string Scenario::rowDescription() const
{
    std::stringstream ss;
    ss << "#";
    ss << " <max coalition size>";
    ss << " <actors>";
    ss << " <agent space: composite actors>";
    ss << " <interfaces>";
    ss << " <valid links>";
    ss << " <invalid links>";
    ss << " <link space: bound on composite actors>";
    ss << " <agent space: composite actor types>";
    ss << " <link space: composite actor types>";
    ss << " <agent space: bound on composite actor types>";
    ss << std::endl;
    return ss.str();
}

std::string Scenario::report() const
{
    std::stringstream ss;
    ss << mMaxCoalitionSize;
    ss << " " << mActors.size();
    ss << " " << mNumberOfActorCombinations;
    ss << " " << mInterfaces.size();
    ss << " " << mValidLinks.size();
    ss << " " << mInvalidLinks.size();
    ss << " " << mNumberOfLinkCombinations;
    ss << " " << mNumberOfActorTypesAgentSpace;
    ss << " " << mNumberOfActorTypesLinkSpace;
    ss << " " << mNumberOfActorTypesTheoreticalBound;
    ss << std::endl;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Scenario& scenario)
{
    os << "# actors:                   " << scenario.mActors.size()
       << std::endl;
    os << "# max actor combos:         " << scenario.mNumberOfActorCombinations
       << std::endl;
    os << "# interfaces:               " << scenario.mInterfaces.size()
       << std::endl;
    os << "# valid links:              " << scenario.mValidLinks.size()
       << std::endl;
    os << "# invalid links:            " << scenario.mInvalidLinks.size()
       << std::endl;
    os << "# valid link combos:        " << scenario.mNumberOfLinkCombinations
       << std::endl;
    os << "# valid link types:         " << scenario.mValidLinkTypes.size()
       << std::endl;
    os << "# valid agents (agent space)"
       << scenario.mNumberOfActorTypesAgentSpace << std::endl;
    os << "# valid agents (link space) "
       << scenario.mNumberOfActorTypesLinkSpace << std::endl;

    std::map<LinkType, size_t>::const_iterator cit =
        scenario.mValidLinkTypes.begin();
    for(; cit != scenario.mValidLinkTypes.end(); ++cit)
    {
        os << "    " << cit->first << " -- " << cit->second << std::endl;
    }
    os << "# invalid link types: " << scenario.mInvalidLinkTypes.size()
       << std::endl;
    return os;
}

} // namespace ccf
} // end namespace multiagent
