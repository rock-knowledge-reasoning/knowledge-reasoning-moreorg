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

typedef uint8_t ActorType;
typedef uint8_t LocalActorId;
typedef uint16_t ActorId;

typedef uint8_t InterfaceType;
typedef uint8_t LocalInterfaceId;
typedef uint32_t InterfaceId;

class Actor
{
    ActorType mType;
    LocalActorId mLocalId;

public:
    Actor() {}

    Actor(ActorType type, LocalActorId id)
        : mType(type)
        , mLocalId(id)
    {}

    bool operator<(const Actor& other) const
    {
        if(mType < other.mType)
        {
            return true;
        } else if(mType == other.mType)
        {
            return mLocalId < other.mLocalId;
        } else {
            return false;
        }
    }

    bool operator==(const Actor& other) const
    {
        return mType == other.mType && mLocalId == other.mLocalId;
    }

    ActorType getType() const { return mType; }
    LocalActorId getLocalId() const { return mLocalId; }
};

std::ostream& operator<<(std::ostream& os, const Actor& a)
{
    os << a.getType() << (char) (a.getLocalId() + 48);
    return os;
}

class Interface
{
    Actor mActor;
    LocalInterfaceId mLocalId;
    InterfaceType mType;
    
public:
    Interface() {}

    Interface(Actor actor, LocalInterfaceId interfaceId, InterfaceType type)
        : mActor(actor)
        , mLocalId(interfaceId)
        , mType(type)
    {}

    Actor getActor() const { return mActor; }
    LocalInterfaceId getLocalId() const { return mLocalId; } 
    InterfaceType getType() const { return mType; }

    bool operator<(const Interface& other) const
    {
        if(mActor < other.mActor)
        {
            return true;
        } else if(mActor == other.mActor)
        {
            return mLocalId < other.mLocalId;
        } else {
            return false;
        }
    }

    bool operator==(const Interface& other) const
    {
        return mActor == other.mActor && mLocalId == other.mLocalId;
    }

};

std::ostream& operator<<(std::ostream& os, const Interface& i)
{
    os << i.getActor() << ":" << (char) (i.getLocalId() + 48) << "[" << (char) (i.getType()) << "]";
    return os;
}

class LinkGroup
{
    Actor mFirstActor;
    Actor mSecondActor;

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
    }

    bool operator<(const LinkGroup& other) const
    {
        if(mFirstActor < other.mFirstActor)
        {
            return true;
        } else if(mFirstActor == other.mFirstActor)
        {
            return mSecondActor < other.mSecondActor;
        } else {
            return false;
        }
    }
    bool operator==(const LinkGroup& other) const
    {
        return mFirstActor == other.mFirstActor && mSecondActor == other.mSecondActor;
    }

    Actor getFirstActor() const { return mFirstActor; }
    Actor getSecondActor() const { return mSecondActor; }
};

std::ostream& operator<<(std::ostream& os, const LinkGroup& linkGroup)
{
    os << linkGroup.getFirstActor() << "--" << linkGroup.getSecondActor();
    return os;
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

    LinkGroup getLinkGroup() const 
    {    
        return LinkGroup(mFirst.getActor(), mSecond.getActor());
    }

    bool operator<(const Link& other) const
    {
        if(other.mFirst < mFirst)
        {
            return true;
        } else if(other.mFirst == mFirst)
        {
            return other.mSecond < mSecond;
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

    CombinedActor(const Link& link, const std::set<LinkGroup>& linkGroups, LinkGroupMap* linkGroupMap, InterfaceLinkMap* interfaceLinkMap)
        : mLinks()
        , mWhiteListLinkGroup(linkGroups)
        , mLinkGroupMap(linkGroupMap)
        , mInterfaceLinkMap(interfaceLinkMap)
    {
        addLink(link); 
    }

    void addLink(const Link& link)
    {
        std::pair< std::set<Link>::const_iterator, bool> result = mLinks.insert(link);
        if(!result.second)
        {
            throw std::runtime_error("Link already exists");
        }

        blacklist(link);
    }

    bool operator<(const CombinedActor& other) const
    {
        return mLinks < other.mLinks;
    }

    /**
     * Return a set of probably valid links -- does not remove the blacklist here, but
     * when trying to add the link
     */
    std::set<Link> getWhitelistLinks() const
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

    void blacklist(const Link& link)
    {
        {
            LinkGroup group = link.getLinkGroup();

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

    std::set<LinkGroup> getWhiteListLinkGroup() const { return mWhiteListLinkGroup; }



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

} // end namespace caf
} // end namespace multiagent
#endif // MULTIAGENT_CAF_HPP
