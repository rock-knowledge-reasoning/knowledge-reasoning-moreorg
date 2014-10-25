#include "Interface.hpp"

namespace multiagent {
namespace ccf {

Interface::Interface() {}

Interface::Interface(Actor actor, LocalInterfaceId interfaceId, CompatibilityType type)
    : mActor(actor)
    , mLocalId(interfaceId)
    , mCompatibilityType(type)
    , mType(actor.getType(), interfaceId)
{}

bool Interface::operator<(const Interface& other) const
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

bool Interface::operator==(const Interface& other) const
{
    return mActor == other.mActor && mLocalId == other.mLocalId;
}

std::ostream& operator<<(std::ostream& os, const Interface& i)
{
    os << i.getActor() << ":" << (char) (i.getLocalId() + 48) << "[" << (char) (i.getCompatibilityType()) << "]";
    return os;
}

} // end namespace ccf
} // end namespace multiagent
