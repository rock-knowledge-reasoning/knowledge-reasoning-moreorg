#include "Interface.hpp"

namespace multiagent {
namespace ccf {

Interface::Interface() {}

Interface::Interface(Actor actor, LocalInterfaceId interfaceId, InterfaceType type)
    : mActor(actor)
    , mLocalId(interfaceId)
    , mType(type)
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
    os << i.getActor() << ":" << (char) (i.getLocalId() + 48) << "[" << (char) (i.getType()) << "]";
    return os;
}

} // end namespace ccf
} // end namespace multiagent
