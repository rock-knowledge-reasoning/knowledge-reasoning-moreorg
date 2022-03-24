#include "AllSelection.hpp"

namespace moreorg {
namespace policies {

Selection AllSelection::apply(const Selection& agents,
                              const OrganizationModelAsk& ask) const
{
    Agent all;
    for(const Agent& a : agents)
    {
        const AtomicAgent::Set s = a.getAtomicAgents();
        for(const AtomicAgent& aa : s)
        {
            all.add(aa);
        }
    }

    return Selection({all});
}

} // end namespace policies
} // end namespace moreorg
