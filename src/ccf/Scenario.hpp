#ifndef MULTIAGENT_CCF_SCENARIO_HPP
#define MULTIAGENT_CCF_SCENARIO_HPP

#include <map>
#include <moreorg/ModelPool.hpp>
#include <moreorg/ccf/Link.hpp>
#include <set>
#include <vector>

namespace multiagent {
namespace ccf {

class ActorDescription
{
    std::map<CompatibilityType, size_t> mInterfaceCount;

public:
    void setInterfaceCount(CompatibilityType type, size_t count)
    {
        mInterfaceCount[type] = count;
    }
    size_t getInterfaceCount(CompatibilityType type)
    {
        return mInterfaceCount[type];
    }
};

class Scenario
{
    friend std::ostream& operator<<(std::ostream& os, const Scenario& scenario);

    size_t mMaxCoalitionSize;
    std::map<char, size_t> mModelPool;

    std::vector<ActorType> mActorTypes;
    std::vector<CompatibilityType> mInterfaceCompatibilityTypes;
    std::map<CompatibilityType, std::vector<CompatibilityType>>
        mInterfaceCompatibility;

    std::map<Actor, ActorDescription> mActors;
    std::vector<Actor> mActorList;
    /// The maximum number of composite agent instances (actor combinations up)
    uint32_t mNumberOfActorCombinations;
    std::vector<Interface> mInterfaces;

    std::vector<Link> mValidLinks;
    std::vector<Link> mInvalidLinks;
    uint32_t mNumberOfLinkCombinations;
    uint32_t mNumberOfActorTypesAgentSpace;
    uint32_t mNumberOfActorTypesLinkSpace;
    uint32_t mNumberOfActorTypesTheoreticalBound;

    // Record how many link of a type do actually exist, i.e.
    // the distribution
    std::map<LinkType, size_t> mValidLinkTypes;
    std::set<LinkType> mInvalidLinkTypes;

    // All links that connect the same actors
    std::map<LinkGroup, std::set<Link>> mLinkGroupMap;
    std::set<LinkGroup> mAvailableLinkGroups;

    std::map<Actor, std::set<Link>> mActorLinkMap;
    std::map<Interface, std::set<Link>> mInterfaceLinkMap;

protected:
    void createLinks();

    std::vector<LinkType> getValidLinkTypeList() const;

public:
    Scenario();
    void compute(size_t maxCoalitionSize);

    std::vector<Link> getValidLinks() { return mValidLinks; }
    std::vector<Link> getInvalidLinks() { return mInvalidLinks; }

    std::map<LinkType, size_t> getValidLinkTypes() { return mValidLinkTypes; }
    std::set<LinkType> getInvalidLinkTypes() { return mInvalidLinkTypes; }

    std::map<LinkGroup, std::set<Link>> getLinkGroupMap() const
    {
        return mLinkGroupMap;
    }
    std::set<LinkGroup> getAvailableLinkGroups() const
    {
        return mAvailableLinkGroups;
    }
    std::map<Actor, std::set<Link>> getActorLinkMap() const
    {
        return mActorLinkMap;
    }
    std::map<Interface, std::set<Link>> getInterfaceLinkMap() const
    {
        return mInterfaceLinkMap;
    }

    void collectCombinations(const std::vector<Actor>& allowedActors,
                             std::vector<Actor> actors,
                             std::set<Link> links,
                             moreorg::ModelPool::Set& agentSpaceModelPools,
                             std::set<std::set<Link>>& linkSpaceSets);

    size_t getNumberOfActors() const { return mActors.size(); }
    static Scenario fromConsole();
    static Scenario fromFile(const std::string& filename);

    std::string rowDescription() const;
    std::string report() const;

    void setModelCount(char key, uint32_t value) { mModelPool[key] = value; }
};

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_SCENARIO_HPP
