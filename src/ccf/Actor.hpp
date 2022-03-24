#ifndef MULTIAGENT_CCF_ACTOR_HPP
#define MULTIAGENT_CCF_ACTOR_HPP

#include <iostream>
#include <stdint.h>

namespace multiagent {
namespace ccf {

using ActorType = uint8_t;
using LocalActorId = uint8_t;
using ActorId = uint16_t;

class Actor
{
    ActorType mType;
    LocalActorId mLocalId;

public:
    Actor();
    Actor(ActorType type, LocalActorId id);

    bool operator<(const Actor& other) const;
    bool operator==(const Actor& other) const;

    ActorType getType() const { return mType; }
    LocalActorId getLocalId() const { return mLocalId; }
};

std::ostream& operator<<(std::ostream& os, const Actor& a);

} // namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_ACTOR_HPP
