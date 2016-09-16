#include "CombinedActor.hpp"

#include <algorithm>
#include <numeric/Combinatorics.hpp>
#include <base-logging/Logging.hpp>
#include <base/Time.hpp>

namespace multiagent {
namespace ccf {

CombinedActor::CombinedActor() {}

CombinedActor::CombinedActor(const Link& link, const std::set<LinkGroup>& linkGroups, LinkGroupMap* linkGroupMap, InterfaceLinkMap* interfaceLinkMap)
    : mLinks()
    , mWhiteListLinkGroup(linkGroups)
    , mLinkGroupMap(linkGroupMap)
    , mInterfaceLinkMap(interfaceLinkMap)
{
    addLink(link); 
}

void CombinedActor::addLink(const Link& link)
{
    std::pair< std::set<Link>::const_iterator, bool> result = mLinks.insert(link);
    if(!result.second)
    {
        throw std::runtime_error("Link already exists");
    }

    blacklist(link);
}

bool CombinedActor::operator<(const CombinedActor& other) const
{
    return mLinks < other.mLinks;
}

/**
 * Return a set of probably valid links -- does not remove the blacklist here, but
 * when trying to add the link
 */
std::set<Link> CombinedActor::getWhitelistLinks() const
{
    std::set<Link> whitelistLinks;
    base::Time start = base::Time::now();
    {
        std::set<LinkGroup>::const_iterator cit = mWhiteListLinkGroup.begin();
        for(; cit != mWhiteListLinkGroup.end(); ++cit)
        {
            const std::set<Link>& linkSet = (*mLinkGroupMap)[*cit];
            std::set<Link>::const_iterator linkSetIt = linkSet.begin();
            for(; linkSetIt != linkSet.end(); ++linkSetIt)
            {
                whitelistLinks.insert(*linkSetIt);
            }
        }
    }
    //std::sort(whitelistLinks.begin(), whitelistLinks.end());
    return whitelistLinks;

//    std::vector<Link> blacklistLinks;
//    {
//        std::set<Interface>::const_iterator cit = mBlackListInterface.begin();
//        for(; cit != mBlackListInterface.end(); ++cit)
//        {
//            const std::set<Link>& linkSet = (*mInterfaceLinkMap)[cit->getId()];
//            blacklistLinks.insert(blacklistLinks.begin(), linkSet.begin(), linkSet.end());
//        }
//    }
//    std::sort(blacklistLinks.begin(), blacklistLinks.end());

//    std::vector<Link> result(whitelistLinks.size());
//    std::vector<Link>::iterator it;
//    it = std::set_difference(whitelistLinks.begin(), whitelistLinks.end(), blacklistLinks.begin(), blacklistLinks.end(), result.begin());
//    result.resize(it - result.begin());
//    LOG_DEBUG_S << (base::Time::now() - start).toSeconds();
//    //std::cout << "GetWhiteLinks" << std::endl;
//    //std::cout << "    white: " << whitelistLinks << std::endl;
//    //std::cout << "    black: " << blacklistLinks << std::endl;
//    //std::cout << "    diff: " << result << std::endl;
//    return result;
}

void CombinedActor::blacklist(const Link& link)
{
    {
        LinkGroup group = link.getGroup();

        // Actors / LinkGroup
        mBlackListLinkGroup.insert(group);
        mWhiteListLinkGroup.erase(group);

        // Interfaces
        {
            std::pair< std::set<Interface>::const_iterator, bool> result = mBlackListInterface.insert(link.getFirstInterface());
            if(!result.second)
            {
                throw std::runtime_error("Link invalid: first interface already blacklisted");
            }
        }
        {
            std::pair< std::set<Interface>::const_iterator, bool> result = mBlackListInterface.insert(link.getSecondInterface());
            if(!result.second)
            {
                throw std::runtime_error("Link invalid: second interface already blacklisted");
            }
        }
        mWhiteListInterface.erase(link.getFirstInterface());
        mWhiteListInterface.erase(link.getSecondInterface());
    }
}

std::ostream& operator<<(std::ostream& os, const CombinedActor& actor)
{
    std::set<Link> links = actor.getLinks();
    os << "Links: [";
    BOOST_FOREACH(const Link& link, links)
    {
        os << link;
        os << ",";
    }
    os << "]" << std::endl;
    os << "     Constraints: " << std::endl;
    os << "         link group:" << std::endl;
    os << "             blacklist: [";
    BOOST_FOREACH(const LinkGroup& group, actor.mBlackListLinkGroup)
    {
        os << group;
        os << ",";
    }
    os << "]" << std::endl;
    os << "             whitelist: [";
    BOOST_FOREACH(const LinkGroup& group, actor.mWhiteListLinkGroup)
    {
        os << group;
        os << ",";
    }
    os << "]" << std::endl;

    os << "         interface:" << std::endl;
    os << "             blacklist: [";
    BOOST_FOREACH(const Interface& interface, actor.mBlackListInterface)
    {
        os << interface;
        os << ",";
    }
    os << "]" << std::endl;
    os << "             whitelist: [";
    BOOST_FOREACH(const Interface& interface, actor.mWhiteListInterface)
    {
        os << interface;
        os << ",";
    }
    os << "]" << std::endl;

    std::set<Link> whitelistLinks = actor.getWhitelistLinks();
    os << "    Whitelist links: [";
    BOOST_FOREACH(const Link& link, whitelistLinks)
    {
        os << link;
        os << ",";
    }
    os << "]" << std::endl;

    return os;
}

} // end namespace ccf
} // end namespace multiagent
