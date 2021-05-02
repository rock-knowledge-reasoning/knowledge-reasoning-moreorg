#ifndef MOREORG_POLICIES_AGENT_SIZE_BASED_SELECTION_HPP
#define MOREORG_POLICIES_AGENT_SIZE_BASED_SELECTION_HPP

#include "SelectionPolicy.hpp"

namespace moreorg {
namespace policies {

class AgentSizeBasedSelection : public SelectionPolicy
{
public:
    AgentSizeBasedSelection(const owlapi::model::IRI& comparisonRelation, size_t value);

    virtual ~AgentSizeBasedSelection() = default;

    Selection apply(const Selection& agent,
            const OrganizationModelAsk& ask) const override;

protected:
    owlapi::model::IRI mOperator;
    size_t mValue;

};

} // end namespace policies
} // end namespace moreorg
#endif // MOREORG_POLICIES_AGENT_SIZE_BASED_SELECTION_HPP
