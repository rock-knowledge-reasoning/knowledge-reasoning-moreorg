#ifndef MULTIAGENT_CCF_LINK_TYPE_HPP
#define MULTIAGENT_CCF_LINK_TYPE_HPP

#include <iostream>
#include <owl_om/ccf/Actor.hpp>
#include <owl_om/ccf/Interface.hpp>

namespace multiagent {
namespace ccf {

class LinkType
{
    friend std::ostream& operator<<(std::ostream& os, const LinkType& linkType);

    InterfaceType mFirstInterfaceType;
    InterfaceType mSecondInterfaceType;

public:
    LinkType();
    LinkType(InterfaceType type0, InterfaceType type1);

    bool operator<(const LinkType& other) const;
};

std::ostream& operator<<(std::ostream& os, const LinkType& linkType);

} // end namespace ccf
} // end namespace multiagent
#endif // MULTIAGENT_CCF_LINK_TYPE_HPP
