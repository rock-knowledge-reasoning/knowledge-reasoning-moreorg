#ifndef MULTIAGENT_CCF_INTERFACE_TYPE_HPP
#define MULTIAGENT_CCF_INTERFACE_TYPE_HPP

#include <moreorg/ccf/Actor.hpp>
#include <ostream>
#include <stdint.h>

namespace multiagent {
namespace ccf {

typedef uint8_t LocalInterfaceId;

class InterfaceType
{
    friend std::ostream& operator<<(std::ostream& os,
                                    const InterfaceType& interfaceType);

    ActorType mActorType;
    LocalInterfaceId mLocalInterfaceId;

public:
    InterfaceType();
    InterfaceType(ActorType actorType, LocalInterfaceId localInterfaceid);

    bool operator<(const InterfaceType& other) const;
    bool operator==(const InterfaceType& other) const;
};

std::ostream& operator<<(std::ostream& os, const InterfaceType& interfaceType);

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_INTERFACE_TYPE_HPP
