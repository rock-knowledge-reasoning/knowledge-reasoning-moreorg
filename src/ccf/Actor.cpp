#include "Actor.hpp"

namespace multiagent {
namespace ccf {

Actor::Actor() {}

Actor::Actor(ActorType type, LocalActorId id)
    : mType(type)
    , mLocalId(id)
{
}

bool Actor::operator<(const Actor& other) const
{
    if(mType < other.mType)
    {
        return true;
    } else if(mType == other.mType)
    {
        return mLocalId < other.mLocalId;
    } else
    {
        return false;
    }
}

bool Actor::operator==(const Actor& other) const
{
    return mType == other.mType && mLocalId == other.mLocalId;
}

std::ostream& operator<<(std::ostream& os, const Actor& a)
{
    os << a.getType() << (char)(a.getLocalId() + 48);
    return os;
}

} // end namespace ccf
} // end namespace multiagent
