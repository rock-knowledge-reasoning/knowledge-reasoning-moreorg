#include "Scenario.hpp"
#include <numeric/Combinatorics.hpp>
#include <numeric/LimitedCombination.hpp>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

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
            boost::tokenizer< boost::char_separator<char> > tokens(line, sep);

            std::vector<std::string> columns(tokens.begin(), tokens.end());
            if(columns.size() != 3)
            {
                std::invalid_argument("organization_model::ccf::Scenario: invalid column: '" + line + "' in spec");
            }

            size_t numberOfInstances = boost::lexical_cast<size_t>(columns.at(0));
            size_t numberOfMaleInterfaces = boost::lexical_cast<size_t>(columns.at(1));
            size_t numberOfFemaleInterfaces = boost::lexical_cast<size_t>(columns.at(2));


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

            ++actorType;
        }
        specFile.close();
    } else {
        throw std::runtime_error("organization_model::ccf::Scenario: failed to load spec from: " + filename );
    }

    return spec;
}

void Scenario::compute(size_t maxCoalitionSize)
{
    mMaxCoalitionSize = maxCoalitionSize;

    std::vector<Actor> mActorList;
    std::map<Actor, ActorDescription>::const_iterator ait = mActors.begin();
    for(; ait != mActors.end(); ++ait)
    {
        Actor actor = ait->first;
        ActorDescription actorDescription = ait->second;

        LocalInterfaceId localInterfaceId = 0;


        // Collect all interfaces (actor, localinterface, interface type)
        // Add interfaces per actor
        std::vector<CompatibilityType>::const_iterator iit = mInterfaceCompatibilityTypes.begin();
        for(; iit != mInterfaceCompatibilityTypes.end(); ++iit)
        {
            // Initialize interfaces
            CompatibilityType interfaceType = *iit;
            for(uint8_t i = 0; i < actorDescription.getInterfaceCount(interfaceType); ++i)
            {
                Interface interface(actor,localInterfaceId++, interfaceType);
                mInterfaces.push_back(interface);
                LOG_DEBUG_S << "Add interface: " << interface << std::endl;
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
    do {
        std::vector<Interface> interfaceCombination = combinations.current();
        Link link(interfaceCombination[0], interfaceCombination[1] );
        // Same type is not compatible
        if(interfaceCombination[0].getCompatibilityType() == interfaceCombination[1].getCompatibilityType())
        {
            mInvalidLinks.push_back(link);
            mInvalidLinkTypes.insert(link.getType());
        } else {
            links.push_back( link );
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

    // Generate number of theoretically possible agent types based (from link instances) space
    Combination<LinkType> linkTypeCombinations( getValidLinkTypeList(), mMaxCoalitionSize-1, MAX);
    mNumberOfLinkTypeCombinations = linkTypeCombinations.numberOfCombinations();

    // Generate number of theoretically possible agent instances (from link instances)
    Combination<Link> linkCombinations( mValidLinks, mMaxCoalitionSize-1, MAX);
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
    ss << " <link space: composite actors>";
    ss << " <link space: composite actor types>";
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
    ss << " " << mNumberOfLinkTypeCombinations;
    ss << std::endl;
    return ss.str();
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
