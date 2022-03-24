#ifndef MULTIAGENT_CCF_LINK_HPP
#define MULTIAGENT_CCF_LINK_HPP

#include <set>
#include <vector>

#include <moreorg/ccf/Actor.hpp>
#include <moreorg/ccf/Interface.hpp>
#include <moreorg/ccf/LinkGroup.hpp>
#include <moreorg/ccf/LinkType.hpp>

namespace multiagent {
namespace ccf {

class Link
{
    // The id of the link -- contains all information
    // about the link
    Interface mFirst;
    Interface mSecond;

public:
    Link();

    Link(Interface first, Interface second);
    Actor getFirstActor() const { return mFirst.getActor(); }
    Actor getSecondActor() const { return mSecond.getActor(); }

    Interface getFirstInterface() const { return mFirst; }
    Interface getSecondInterface() const { return mSecond; }

    LinkGroup getGroup() const;
    LinkType getType() const;

    bool operator<(const Link& other) const;
};

std::ostream& operator<<(std::ostream& os, const Link& link);
std::ostream& operator<<(std::ostream& os, const std::vector<Link>& links);
std::ostream& operator<<(std::ostream& os, const std::set<Link>& links);
std::ostream& operator<<(std::ostream& os,
                         const std::set<std::set<Link>>& links);

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_LINK_HPP
