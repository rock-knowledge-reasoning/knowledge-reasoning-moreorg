#ifndef ORGANIZATION_MODEL_AGENT_LINK_HPP
#define ORGANIZATION_MODEL_AGENT_LINK_HPP

#include "AgentInterface.hpp"
#include <set>

namespace moreorg {

class AgentLink
{
public:
    typedef std::set<AgentLink> Set;

    AgentLink() = default;

    AgentLink(const AgentInterface& source,
            const AgentInterface& target);

private:
    AgentInterface mSource;
    AgentInterface mTarget;

};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_AGENT_LINK_HPP
