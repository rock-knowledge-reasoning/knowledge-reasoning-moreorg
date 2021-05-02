#include "AgentSizeBasedSelection.hpp"
#include "../vocabularies/OM.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace policies {

AgentSizeBasedSelection::AgentSizeBasedSelection(const IRI& comparisonRelation, size_t value)
    : SelectionPolicy(vocabulary::OM::resolve("AgentSizeBasedSelection_" +
                comparisonRelation.getFragment() + "_" +
                std::to_string(value)))
    , mOperator(comparisonRelation)
    , mValue(value)
{}

Selection AgentSizeBasedSelection::apply(const Selection& selection,
    const OrganizationModelAsk& ask) const
{
    Selection updatedSelection;
    for(const Agent& a : selection)
    {
        size_t atomicAgentCount = a.getAtomicAgents().size();

        bool add = false;
        if(mOperator == vocabulary::OM::resolve("equals"))
        {
            add = (atomicAgentCount == mValue);
        } else if(mOperator == vocabulary::OM::resolve("lessThan"))
        {
            add = (atomicAgentCount < mValue);
        } else if(mOperator == vocabulary::OM::resolve("lessOrEqual"))
        {
            add = (atomicAgentCount <= mValue);
        } else if(mOperator == vocabulary::OM::resolve("greaterThan"))
        {
            add = (atomicAgentCount > mValue);
        } else if(mOperator == vocabulary::OM::resolve("greaterOrEqual"))
        {
            add = (atomicAgentCount >= mValue);
        }

        if(add)
        {
            updatedSelection.insert(a);
        }
    }
    return updatedSelection;
}

} // end namespace policies
} // end namespace moreorg
