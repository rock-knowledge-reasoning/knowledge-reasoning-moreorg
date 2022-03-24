#include "AgentLink.hpp"

namespace moreorg {

AgentLink::AgentLink(const AgentInterface& source, const AgentInterface& target)
    : mSource(source)
    , mTarget(target)
{
}

} // end namespace moreorg
