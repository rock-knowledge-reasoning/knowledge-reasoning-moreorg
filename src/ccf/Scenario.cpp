#include "Scenario.hpp"
#include <owl_om/Combinatorics.hpp>

namespace multiagent {
namespace ccf {

Scenario::Scenario()
{
    mInterfaceTypes.push_back('m');
    mInterfaceTypes.push_back('f');
}


Scenario Scenario::fromConsole()
{
    Scenario scenario;

    size_t numberOfActorTypes;
    std::cout << "How many different actor types are being used:" << std::endl;
    std::cin >> numberOfActorTypes;

    char currentActorType = 'a';
    std::cout << "Please provide information about the actor types: " << std::endl;

    for(int i = 0; i < numberOfActorTypes; ++i)
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
    std::map<Actor, ActorDescription>::const_iterator ait = mActors.begin();
    for(; ait != mActors.end(); ++ait)
    {
        Actor actor = ait->first;
        ActorDescription actorDescription = ait->second;

        LocalInterfaceId localInterfaceId = 0;

        // Add interfaces per actor
        std::vector<InterfaceType>::const_iterator iit = mInterfaceTypes.begin();
        for(; iit != mInterfaceTypes.end(); ++iit)
        {
            // Initialize interfaces
            InterfaceType interfaceType = *iit;
            for(uint8_t i = 0; i < actorDescription.getInterfaceCount(interfaceType); ++i)
            {
                Interface interface(actor,localInterfaceId++, *iit);
                mInterfaces.push_back(interface);
                std::cout << "Add interface: " << interface << std::endl;
            }
        }
    }

    createLinks();
}

void Scenario::createLinks()
{
    std::vector<Link> links;
    base::combinatorics::Combination<Interface> combinations(mInterfaces, 2, base::combinatorics::EXACT);

    do {
        std::vector<Interface> interfaceCombination = combinations.current();
        Link link(interfaceCombination[0], interfaceCombination[1] );
        if(interfaceCombination[0].getType() == interfaceCombination[1].getType())
        {
            mInvalidLinks.push_back(link);
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
            continue;
        }
        mValidLinks.push_back(link);

        // Create link groups
        mLinkGroupMap[link.getLinkGroup()].insert(link);
        mAvailableLinkGroups.insert(link.getLinkGroup());

        // Create interface mapping
        mInterfaceLinkMap[link.getFirstInterface()].insert(link);
        mInterfaceLinkMap[link.getSecondInterface()].insert(link);

        // Create actor mapping
        mActorLinkMap[link.getFirstActor()].insert(link);
        mActorLinkMap[link.getSecondActor()].insert(link);
    }   
}

std::ostream& operator<<(std::ostream& os, const Scenario& scenario)
{
    os << "# actors:           " << scenario.mActors.size() << std::endl;
    os << "# interfaces:       " << scenario.mInterfaces.size() << std::endl;
    os << "# valid links:      " << scenario.mValidLinks.size() << std::endl;
    os << "# invalid links:    " << scenario.mInvalidLinks.size() << std::endl;
    return os;
}

} // end namespace caf
} // end namespace multiagent
