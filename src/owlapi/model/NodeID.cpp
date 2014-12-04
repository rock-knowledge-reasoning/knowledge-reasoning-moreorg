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
    if(anonymous)
    {
        if(boost::starts_with(id, "_:"))
        {
            mId = id;
        } else {
            mId = "_:" + id;
        }
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
