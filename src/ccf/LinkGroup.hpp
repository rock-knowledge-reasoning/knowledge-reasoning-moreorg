#ifndef MULTIAGENT_CCF_LINKGROUP_HPP
#define MULTIAGENT_CCF_LINKGROUP_HPP

#include <owl_om/ccf/Actor.hpp>

namespace multiagent {
namespace ccf {

class LinkGroup
{
    Actor mFirstActor;
    Actor mSecondActor;

public:
    LinkGroup();
    LinkGroup(Actor actor0, Actor actor1);

    bool operator<(const LinkGroup& other) const;
    bool operator==(const LinkGroup& other) const;

    Actor getFirstActor() const { return mFirstActor; }
    Actor getSecondActor() const { return mSecondActor; }
};

std::ostream& operator<<(std::ostream& os, const LinkGroup& linkGroup);

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_LINKGROUP_HPP
