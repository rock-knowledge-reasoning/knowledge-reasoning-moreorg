#ifndef MULTIAGENT_COMBINED_ACTOR_HPP
#define MULTIAGENT_COMBINED_ACTOR_HPP

#include <set>
#include <vector>
#include <map>
#include <organization_model/ccf/Link.hpp>
#include <organization_model/ccf/LinkGroup.hpp>

namespace multiagent {
namespace ccf {

class CombinedActor
{

    friend std::ostream& operator<<(std::ostream& os, const CombinedActor& actor);

protected:
    std::set<Link> mLinks;

    typedef std::map<LinkGroup, std::set<Link> > LinkGroupMap;
    LinkGroupMap* mLinkGroupMap;
    typedef std::map<Interface, std::set<Link> > InterfaceLinkMap;
    InterfaceLinkMap* mInterfaceLinkMap;

    std::set<LinkGroup> mBlackListLinkGroup;
    std::set<LinkGroup> mWhiteListLinkGroup;

    std::set<Interface> mBlackListInterface;
    std::set<Interface> mWhiteListInterface;

public:
    CombinedActor();

    CombinedActor(const Link& link, const std::set<LinkGroup>& linkGroups, LinkGroupMap* linkGroupMap, InterfaceLinkMap* interfaceLinkMap);

    void addLink(const Link& link);
    bool operator<(const CombinedActor& other) const;

    /**
     * Return a set of probably valid links -- does not remove the blacklist here, but
     * when trying to add the link
     */
    std::set<Link> getWhitelistLinks() const;
    void blacklist(const Link& link);

    std::set<LinkGroup> getWhiteListLinkGroup() const { return mWhiteListLinkGroup; } 
    std::set<Interface> getWhiteListInterface() const { return mWhiteListInterface; }

    std::set<Link> getLinks() const { return mLinks; } 
};

std::ostream& operator<<(std::ostream& os, const CombinedActor& actor);

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_COMBINED_ACTOR_HPP
