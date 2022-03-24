#include "LinkType.hpp"

namespace multiagent {
namespace ccf {

LinkType::LinkType() {}

LinkType::LinkType(InterfaceType type0, InterfaceType type1)
{
    if(type0 < type1)
    {
        mFirstInterfaceType = type0;
        mSecondInterfaceType = type1;
    } else
    {
        mFirstInterfaceType = type1;
        mSecondInterfaceType = type0;
    }
}

bool LinkType::operator<(const LinkType& other) const
{
    if(mFirstInterfaceType < other.mFirstInterfaceType)
    {
        return true;
    } else if(mFirstInterfaceType == other.mFirstInterfaceType)
    {
        return mSecondInterfaceType < other.mSecondInterfaceType;
    } else
    {
        return false;
    }
}

std::ostream& operator<<(std::ostream& os, const LinkType& linkType)
{
    os << linkType.mFirstInterfaceType << "--" << linkType.mSecondInterfaceType;
    return os;
}

} // end namespace ccf
} // end namespace multiagent
