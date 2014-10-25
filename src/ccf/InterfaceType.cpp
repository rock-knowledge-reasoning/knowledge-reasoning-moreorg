#include "InterfaceType.hpp"

namespace multiagent {
namespace ccf {

InterfaceType::InterfaceType()
{}

InterfaceType::InterfaceType(ActorType actorType, LocalInterfaceId localInterfaceId)
    : mActorType(actorType)
    , mLocalInterfaceId(localInterfaceId)
{}

bool InterfaceType::operator<(const InterfaceType& other) const
{
    if(mActorType < other.mActorType)
    {
        return true;
    } else if(mActorType == other.mActorType)
    {
        return mLocalInterfaceId < other.mLocalInterfaceId;
    } else {
        return false;
    }
}

bool InterfaceType::operator==(const InterfaceType& other) const
{
    return mActorType == other.mActorType && mLocalInterfaceId == other.mLocalInterfaceId;
}

std::ostream& operator<<(std::ostream& os, const InterfaceType& interfaceType)
{
    os << interfaceType.mActorType << (char) (interfaceType.mLocalInterfaceId + 48);
    return os;
}

} // end namespace ccf
} // end namespace multiagent
