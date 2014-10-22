#ifndef MULTIAGENT_CCF_INTERFACE_HPP
#define MULTIAGENT_CCF_INTERFACE_HPP

#include <owl_om/ccf/Actor.hpp>

namespace multiagent {
namespace ccf {

typedef uint8_t InterfaceType;
typedef uint8_t LocalInterfaceId;
typedef uint32_t InterfaceId;

class Interface
{
    Actor mActor;
    LocalInterfaceId mLocalId;
    InterfaceType mType;
    
public:
    Interface();
    Interface(Actor actor, LocalInterfaceId interfaceId, InterfaceType type);

    Actor getActor() const { return mActor; }
    LocalInterfaceId getLocalId() const { return mLocalId; } 
    InterfaceType getType() const { return mType; }

    bool operator<(const Interface& other) const;
    bool operator==(const Interface& other) const;

};

std::ostream& operator<<(std::ostream& os, const Interface& i);

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_INTERFACE_HPP
