#include "NodeID.hpp"
#include <boost/algorithm/string.hpp>

namespace owlapi {
namespace model {

uint32_t NodeID::msGlobalId = 0;

NodeID::NodeID()
    : mId()
    , mAnonymous(false)
{}

NodeID::NodeID(const std::string& id, bool anonymous)
    : mAnonymous(anonymous)
{
    if(boost::starts_with(id, "_:"))
    {
        mId = "_:" + id;
    } else {
        mId = id;
    }
}

std::string NodeID::nextAnonymousIRI()
{
    return nodeString(msGlobalId);
}

std::string NodeID::nodeString(uint32_t id)
{
    std::stringstream ss;
    ss << id;
    return ss.str();
}

} // end namespace model
} // end namespace owlapi
