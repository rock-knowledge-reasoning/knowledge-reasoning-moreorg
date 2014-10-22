#ifndef MULTIAGENT_CAF_HPP
#define MULTIAGENT_CAF_HPP

#include <stdint.h>
#include <assert.h>
#include <base/Time.hpp>
#include <set>
#include <vector>
#include <algorithm>
#include <owl_om/Combinatorics.hpp>
#include <base/Logging.hpp>

namespace multiagent {
namespace caf {

typedef uint8_t Actor;
typedef uint8_t LocalInterfaceId;
typedef uint16_t InterfaceId;

class Interface
{
    Actor mActor;
    LocalInterfaceId mLocalInterfaceId;
    InterfaceId mId;
    
public:
    Interface() {}

    Interface(Actor actor, LocalInterfaceId interfaceId)
        : mActor(actor)
        , mLocalInterfaceId(interfaceId)
    {
        mId = mActor;
        mId <<=8;
        mId += mLocalInterfaceId;
    }

    Actor getActor() const { return mActor; }
    LocalInterfaceId getLocalId() const { return mLocalInterfaceId; } 

    bool operator<(const Interface& other) const
    {
        if(other.mActor < mActor)
        {
            return true;
        } else if(other.mActor == mActor)
        {
            return other.mLocalInterfaceId < mLocalInterfaceId;
        } else {
            return false;
        }
    }

    bool operator==(const Interface& other) const
    {
        return mActor == other.mActor && mLocalInterfaceId == other.mLocalInterfaceId;
    }

    InterfaceId getId() const { return mId; }
};

std::ostream& operator<<(std::ostream& os, const Interface& i)
{
    os << i.getActor() << ":" << (char) (i.getLocalId() + 48);
    return os;
}

typedef uint16_t LinkGroupId;
class LinkGroup
{
    Actor mFirstActor;
    Actor mSecondActor;

    LinkGroupId mId;

public:
    LinkGroup() {}

    LinkGroup(Actor actor0, Actor actor1)
        : mFirstActor(actor0)
        , mSecondActor(actor1)
    {
        if(actor0 < actor1)
        {
            mFirstActor = actor0;
            mSecondActor = actor1;
        } else {
            mFirstActor = actor1;
            mSecondActor = actor0;
        }

        mId = mFirstActor;
        mId <<= 8; 
        mId += mSecondActor;
    }

    LinkGroupId getId() const { return mId; }
};

std::string stringifyLinkGroupId(const LinkGroupId& id)
{
    std::stringstream ss;
    uint8_t secondActor = id & 0xFF;
    uint8_t firstActor = (id >> 8) & 0xFF;
    ss << firstActor << ":" << secondActor;
    return ss.str();
}

class Link
{
    // The id of the link -- contains all information
    // about the link
    Interface mFirst;
    Interface mSecond;

public: 
    Link() {}

    Link(Interface first, Interface second)
    {
        if(first < second)
        {
            mFirst = first;
            mSecond = second;
        } else {
            mSecond = first;
            mFirst = second;
        }
    } 

    Actor getFirstActor() const   { return mFirst.getActor(); }
    Actor getSecondActor() const  { return mSecond.getActor(); }

    Interface getFirstInterface() const { return mFirst; }
    Interface getSecondInterface() const  { return mSecond; }

    LinkGroupId getLinkGroupId() const 
    {    
        LinkGroup linkGroup(mFirst.getActor(), mSecond.getActor());
        return linkGroup.getId();
    }

    bool operator<(const Link& other) const
    {
        if(other.mFirst < mFirst)
        {
            return true;
        } else if(other.mFirst == mFirst)
        {
            if(other.mSecond == mSecond)
            {
                return false;
            } else {
                return other.mSecond < mSecond;
            }
        } else {
            return false;
        }
    }

};

std::ostream& operator<<(std::ostream& os, const Link& link)
{
    os << link.getFirstInterface() << " -- " << link.getSecondInterface();
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Link>& links)
{
    BOOST_FOREACH(const Link& link, links)
    {
        os << link;
        os << ",";
    }
    return os;
}



class CombinedActor
{

    friend std::ostream& operator<<(std::ostream& os, const CombinedActor& actor);

protected:
    std::set<Link> mLinks;

    typedef std::map<LinkGroupId, std::set<Link> > LinkGroupMap;
    LinkGroupMap* mLinkGroupMap;
    typedef std::map<InterfaceId, std::set<Link> > InterfaceLinkMap;
    InterfaceLinkMap* mInterfaceLinkMap;

    std::set<LinkGroupId> mBlackListLinkGroup;
    std::set<LinkGroupId> mWhiteListLinkGroup;

    std::set<Interface> mBlackListInterface;
    std::set<Interface> mWhiteListInterface;

public:
    CombinedActor();

    CombinedActor(const Link& link, const std::set<LinkGroupId>& linkGroups, LinkGroupMap* linkGroupMap, InterfaceLinkMap* interfaceLinkMap)
        : mLinks()
        , mWhiteListLinkGroup(linkGroups)
        , mLinkGroupMap(linkGroupMap)
        , mInterfaceLinkMap(interfaceLinkMap)
    {
        addLink(link); 
    }

    void addLink(const Link& link)
    {
        mLinks.insert(link);
        blacklist(link);
    }

    bool operator<(const CombinedActor& other) const
    {
        return mLinks < other.mLinks;
    }

    std::vector<Link> getWhitelistLinks() const
    {
        std::vector<Link> whitelistLinks;
        base::Time start = base::Time::now();
        {
            std::set<LinkGroupId>::const_iterator cit = mWhiteListLinkGroup.begin();
            for(; cit != mWhiteListLinkGroup.end(); ++cit)
            {
                const std::set<Link>& linkSet = (*mLinkGroupMap)[*cit];
                whitelistLinks.insert(whitelistLinks.begin(), linkSet.begin(), linkSet.end());
            }
        }
        std::sort(whitelistLinks.begin(), whitelistLinks.end());

        std::vector<Link> blacklistLinks;
        {
            std::set<Interface>::const_iterator cit = mBlackListInterface.begin();
            for(; cit != mBlackListInterface.end(); ++cit)
            {
                const std::set<Link>& linkSet = (*mInterfaceLinkMap)[cit->getId()];
                blacklistLinks.insert(blacklistLinks.begin(), linkSet.begin(), linkSet.end());
            }
        }
        std::sort(blacklistLinks.begin(), blacklistLinks.end());

        std::vector<Link> result(whitelistLinks.size());
        std::vector<Link>::iterator it;
        it = std::set_difference(whitelistLinks.begin(), whitelistLinks.end(), blacklistLinks.begin(), blacklistLinks.end(), result.begin());
        result.resize(it - result.begin());
        LOG_DEBUG_S << (base::Time::now() - start).toSeconds();
        //std::cout << "GetWhiteLinks" << std::endl;
        //std::cout << "    white: " << whitelistLinks << std::endl;
        //std::cout << "    black: " << blacklistLinks << std::endl;
        //std::cout << "    diff: " << result << std::endl;
        return result;

//        LOG_DEBUG_S << "a: " << (base::Time::now() - start).toSeconds() << " size: " << mWhiteListLinkGroup.size();
//        whitelistLinks.clear();
//
//        BOOST_FOREACH(const LinkGroupId& id, mWhiteListLinkGroup)
//        {
//            const std::set<Link>& linkSet = (*mLinkGroupMap)[id];
//            whitelistLinks.insert(whitelistLinks.begin(), linkSet.begin(), linkSet.end());
//        }
//        LOG_DEBUG_S << "a: " << (base::Time::now() - start).toSeconds() << " size: " << mWhiteListLinkGroup.size();
//
//
//        std::vector<Link> blacklistLinks;
//        BOOST_FOREACH(const Interface& interface, mBlackListInterface)
//        {
//            const std::set<Link>& linkSet = (*mInterfaceLinkMap)[interface.getId()];
//            blacklistLinks.insert(blacklistLinks.begin(), linkSet.begin(), linkSet.end());
//        }
//        LOG_DEBUG_S << "b: " << (base::Time::now() - start).toSeconds() << " size: " << mBlackListLinkGroup.size();
//
//        std::vector<Link> result(whitelistLinks.size());
//        std::vector<Link>::iterator it;
//        it = std::set_difference(whitelistLinks.begin(), whitelistLinks.end(), blacklistLinks.begin(), blacklistLinks.end(), result.begin());
//        result.resize(it - result.begin());
//        LOG_DEBUG_S << "c: " << (base::Time::now() - start).toSeconds();
//
//        //LOG_DEBUG("Difference");
//        //BOOST_FOREACH(Link link, result)
//        //{
//        //    LOG_DEBUG_S << link;
//        //}
//        LOG_DEBUG_S << (base::Time::now() - start).toSeconds();
//        return result;

        //// Backlist all interfaces that are of the same interface groups
        //std::vector<Link> blacklistLinks;
        //BOOST_FOREACH(const Link& link, mLinks)
        //{
        //    {
        //        std::map<InterfaceId, std::set<Link> >::const_iterator cit = interfaceLinkMap.find(link.getFirstInterface().getId());
        //        assert( cit != interfaceLinkMap.end());
        //        const std::set<Link>& interfaceGroupLinks = cit->second;
        //        blacklistLinks.insert(blacklistLinks.end(), interfaceGroupLinks.begin(), interfaceGroupLinks.end());
        //    }
        //    {
        //        std::map<InterfaceId, std::set<Link> >::const_iterator cit = interfaceLinkMap.find(link.getSecondInterface().getId());
        //        assert( cit != interfaceLinkMap.end());
        //        const std::set<Link>& interfaceGroupLinks = cit->second;
        //        blacklistLinks.insert(blacklistLinks.end(), interfaceGroupLinks.begin(), interfaceGroupLinks.end());
        //    }
        //}
        //std::sort(blacklistLinks.begin(), blacklistLinks.end());
        ////LOG_DEBUG("Blacklist: %d", blacklistLinks.size());

        //// Get the whitelist based on the linkgroups
        //std::vector<Link> whitelistLinks;
        //std::map<LinkGroupId, std::set<Link> >::const_iterator lit = linkGroupMap.begin();

        //for(; lit != linkGroupMap.end(); ++lit)
        //{
        //    std::set<Link> linkGroupLinks = lit->second;
        //    whitelistLinks.insert(whitelistLinks.end(), linkGroupLinks.begin(), linkGroupLinks.end());
        //}
        //std::sort(whitelistLinks.begin(), whitelistLinks.end());
        ////LOG_DEBUG("Whitelist: %d", whitelistLinks.size());

        //// The difference of two sets is formed by the elements that are present in the first set, but not in the second one. The elements copied by the function come always from the first range, in the same order.
        //std::vector<Link> result(whitelistLinks.size());
        //if(whitelistLinks.empty())
        //{
        //    return result;
        //} else if(blacklistLinks.empty())
        //{
        //    return whitelistLinks;
        //} else {
        //    //LOG_DEBUG("Compute Difference");
        //    //LOG_DEBUG("Whitelist");
        //    //BOOST_FOREACH(Link link, whitelistLinks)
        //    //{
        //    //    LOG_DEBUG_S << link;
        //    //}
        //    //LOG_DEBUG("Blacklist");
        //    //BOOST_FOREACH(Link link, blacklistLinks)
        //    //{
        //    //    LOG_DEBUG_S << link;
        //    //}

        //    std::vector<Link>::iterator it;
        //    it = std::set_difference(whitelistLinks.begin(), whitelistLinks.end(), blacklistLinks.begin(), blacklistLinks.end(), result.begin());
        //    result.resize(it - result.begin());
        //    //LOG_DEBUG("Difference");
        //    //BOOST_FOREACH(Link link, result)
        //    //{
        //    //    LOG_DEBUG_S << link;
        //    //}
        //    return result;
        //}
    }

    void blacklist(const Link& link)
    {
        {
            LinkGroupId id = link.getLinkGroupId();

            // Actors / LinkGroup
            mBlackListLinkGroup.insert(id);
            mWhiteListLinkGroup.erase(id);

            // Interfaces
            mBlackListInterface.insert(link.getFirstInterface());
            mBlackListInterface.insert(link.getSecondInterface());
            mWhiteListInterface.erase(link.getFirstInterface());
            mWhiteListInterface.erase(link.getSecondInterface());
        }
    }

    std::set<LinkGroupId> getWhiteListLinkGroup() const { return mWhiteListLinkGroup; }



    std::set<Interface> getWhiteListInterface() const {
        return mWhiteListInterface;
    }

    std::set<Link> getLinks() const { return mLinks; } 
};


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
    BOOST_FOREACH(const LinkGroupId& id, actor.mBlackListLinkGroup)
    {
        os << stringifyLinkGroupId(id);
        os << ",";
    }
    os << "]" << std::endl;
    os << "             whitelist: [";
    BOOST_FOREACH(const LinkGroupId& id, actor.mWhiteListLinkGroup)
    {
        os << stringifyLinkGroupId(id);
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

    std::vector<Link> whitelistLinks = actor.getWhitelistLinks();
    os << "    Whitelist links: [";
    BOOST_FOREACH(const Link& link, whitelistLinks)
    {
        os << link;
        os << ",";
    }
    os << "]" << std::endl;

    return os;
}

} // end namespace caf
} // end namespace multiagent
#endif // MULTIAGENT_CAF_HPP
