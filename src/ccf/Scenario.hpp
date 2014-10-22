#ifndef MULTIAGENT_CCF_SCENARIO_HPP
#define MULTIAGENT_CCF_SCENARIO_HPP

#include <map>
#include <set>
#include <vector>
#include <owl_om/ccf/Link.hpp>

namespace multiagent {
namespace ccf {

class ActorDescription
{
    std::map<InterfaceType,size_t> mInterfaceCount;
public:
    void setInterfaceCount(InterfaceType type, size_t count) { mInterfaceCount[type] = count; }
    size_t getInterfaceCount(InterfaceType type) { return mInterfaceCount[type]; }
};

class Scenario
{
    friend std::ostream& operator<<(std::ostream& os, const Scenario& scenario);

    std::vector<ActorType> mActorTypes;
    std::vector<InterfaceType> mInterfaceTypes;
    std::map<InterfaceType, std::vector<InterfaceType> > mInterfaceCompatibility;

    std::map<Actor, ActorDescription> mActors;
    std::vector<Interface> mInterfaces;

    std::vector<Link> mValidLinks;
    std::vector<Link> mInvalidLinks;

    // All links that connect the same actors
    std::map<LinkGroup, std::set<Link> > mLinkGroupMap;
    std::set<LinkGroup> mAvailableLinkGroups;

    std::map<Actor, std::set<Link> > mActorLinkMap;
    std::map<Interface, std::set<Link> > mInterfaceLinkMap;

protected:
    void createLinks();
    void compute();

public:
    Scenario();

    std::map<LinkGroup, std::set<Link> > getLinkGroupMap();
    std::set<LinkGroup> getAvailableLinkGroups();
    std::map<Actor, std::set<Link> > getActorLinkMap();
    std::map<Interface, std::set<Link> > getInterfaceLinkMap();

    static Scenario fromConsole();
};

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_SCENARIO_HPP
