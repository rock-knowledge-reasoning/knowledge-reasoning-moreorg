#include "Scenario.hpp"
#include <owl_om/Combinatorics.hpp>

using namespace base::combinatorics;

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
    std::cout << "Please provide information about the actor types: " << std::endl;

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
        actorDescription.setInterfaceCount('m',maleEmiCount);

        size_t femaleEmiCount;
        std::cout << "The available number of EMI- for this type:" << std::endl;
        std::cout << "Female EMI: " << std::endl;
        std::cin >> femaleEmiCount;
        actorDescription.setInterfaceCount('f',femaleEmiCount);
        scenario.mActorTypes.push_back(currentActorType);

        uint8_t localActorId = 0;
        // Add interfaces per actor
        for(size_t i = 0; i < numberPerActorType; ++i)
        {
            Actor actor(currentActorType, localActorId++);
            scenario.mActors[actor] = actorDescription;
        }
        ++currentActorType;
    }

    scenario.compute();

    return scenario;
}

void Scenario::compute()
{
    std::vector<Actor> actorList;
    std::map<Actor, ActorDescription>::const_iterator ait = mActors.begin();
    for(; ait != mActors.end(); ++ait)
    {
        Actor actor = ait->first;
        ActorDescription actorDescription = ait->second;

        LocalInterfaceId localInterfaceId = 0;

        // Add interfaces per actor
        std::vector<CompatibilityType>::const_iterator iit = mInterfaceCompatibilityTypes.begin();
        for(; iit != mInterfaceCompatibilityTypes.end(); ++iit)
        {
            // Initialize interfaces
            CompatibilityType interfaceType = *iit;
            for(uint8_t i = 0; i < actorDescription.getInterfaceCount(interfaceType); ++i)
            {
                Interface interface(actor,localInterfaceId++, *iit);
                mInterfaces.push_back(interface);
                LOG_DEBUG_S << "Add interface: " << interface << std::endl;
            }
        }

        actorList.push_back(actor);
    }

    
    Combination<Actor> combination(actorList, actorList.size(), MAX);
    mNumberOfActorCombinations = combination.numberOfCombinations();

    createLinks();
}

void Scenario::createLinks()
{
    std::vector<Link> links;
    base::combinatorics::Combination<Interface> combinations(mInterfaces, 2, base::combinatorics::EXACT);

    do {
        std::vector<Interface> interfaceCombination = combinations.current();
        Link link(interfaceCombination[0], interfaceCombination[1] );
        if(interfaceCombination[0].getCompatibilityType() == interfaceCombination[1].getCompatibilityType())
        {
            mInvalidLinks.push_back(link);
            mInvalidLinkTypes.insert(link.getType());
        } else {
            links.push_back( link );
        }
    } while(combinations.next());

    // Remove links that violate general constraints
    BOOST_FOREACH(Link link, links)
    {
        if(link.getFirstActor() == link.getSecondActor())
        {
            mInvalidLinks.push_back(link);
            mInvalidLinkTypes.insert(link.getType());
            continue;
        }
        mValidLinks.push_back(link);
        mValidLinkTypes[link.getType()]++;

        // Create link groups
        mLinkGroupMap[link.getGroup()].insert(link);
        mAvailableLinkGroups.insert(link.getGroup());

        // Create interface mapping
        mInterfaceLinkMap[link.getFirstInterface()].insert(link);
        mInterfaceLinkMap[link.getSecondInterface()].insert(link);

        // Create actor mapping
        mActorLinkMap[link.getFirstActor()].insert(link);
        mActorLinkMap[link.getSecondActor()].insert(link);
    }   

    Combination<LinkType> linkTypeCombinations( getValidLinkTypeList(), mActors.size()-1, MAX);
    mNumberOfLinkTypeCombinations = linkTypeCombinations.numberOfCombinations();

    Combination<Link> linkCombinations( mValidLinks, mActors.size()-1, MAX);
    mNumberOfLinkCombinations = linkCombinations.numberOfCombinations();
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

void Scenario::createCompositeActorTypes()
{
    Combination<LinkType> linkCombinations(getValidLinkTypeList(), mActors.size()-1, MAX);
    do
    {
        //
    } while(linkCombinations.next());
}

std::ostream& operator<<(std::ostream& os, const Scenario& scenario)
{
    os << "# actors:                  " << scenario.mActors.size() << std::endl;
    os << "# max actor combos:        " << scenario.mNumberOfActorCombinations << std::endl;
    os << "# interfaces:              " << scenario.mInterfaces.size() << std::endl;
    os << "# valid links:             " << scenario.mValidLinks.size() << std::endl;
    os << "# invalid links:           " << scenario.mInvalidLinks.size() << std::endl;
    os << "# valid link combos:       " << scenario.mNumberOfLinkCombinations << std::endl;
    os << "# valid link types:        " << scenario.mValidLinkTypes.size() << std::endl;
    os << "# valid link type combos:  " << scenario.mNumberOfLinkTypeCombinations << std::endl;

    std::map<LinkType, size_t>::const_iterator cit = scenario.mValidLinkTypes.begin();
    for(; cit != scenario.mValidLinkTypes.end(); ++cit)
    {
        os << "    " << cit->first << " -- " << cit->second << std::endl;
    }
    os << "# invalid link types: " << scenario.mInvalidLinkTypes.size() << std::endl;
    return os;
}

} // end namespace caf
} // end namespace multiagent
