#ifndef ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP

#include "../Agent.hpp"
#include "../Policy.hpp"

namespace moreorg {
namespace policies {

using Selection = Agent::Set;

class SelectionPolicy : public Policy
{
public:
    using Ptr = shared_ptr<SelectionPolicy>;

    SelectionPolicy(const owlapi::model::IRI& iri = owlapi::model::IRI());

    virtual ~SelectionPolicy() = default;

    /**
     * Selection policies can be chained, so that apply returns an
     * updated selection
     */
    virtual Selection apply(const Selection& agentSelection,
                            const OrganizationModelAsk& ask) const;

    void add(const SelectionPolicy::Ptr& policy)
    {
        mPolicyChain.push_back(policy);
    }

protected:
    std::vector<SelectionPolicy::Ptr> mPolicyChain;
};

} // namespace policies
} // namespace moreorg

#endif // ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP
