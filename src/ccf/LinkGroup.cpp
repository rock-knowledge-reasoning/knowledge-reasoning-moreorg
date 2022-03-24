#include "LinkGroup.hpp"

namespace multiagent {
namespace ccf {

LinkGroup::LinkGroup() {}

LinkGroup::LinkGroup(Actor actor0, Actor actor1)
    : mFirstActor(actor0)
    , mSecondActor(actor1)
{
    if(actor0 < actor1)
    {
        mFirstActor = actor0;
        mSecondActor = actor1;
    } else
    {
        mFirstActor = actor1;
        mSecondActor = actor0;
    }
}

bool LinkGroup::operator<(const LinkGroup& other) const
{
    if(mFirstActor < other.mFirstActor)
    {
        return true;
    } else if(mFirstActor == other.mFirstActor)
    {
        return mSecondActor < other.mSecondActor;
    } else
    {
        return false;
    }
}
bool LinkGroup::operator==(const LinkGroup& other) const
{
    return mFirstActor == other.mFirstActor &&
           mSecondActor == other.mSecondActor;
}

std::ostream& operator<<(std::ostream& os, const LinkGroup& linkGroup)
{
    os << linkGroup.getFirstActor() << "--" << linkGroup.getSecondActor();
    return os;
}

} // namespace ccf
} // end namespace multiagent
