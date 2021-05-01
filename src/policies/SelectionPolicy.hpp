#ifndef ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP

#include "../Policy.hpp"
#include "../Agent.hpp"

namespace moreorg {
namespace policies {

typedef Agent::Set Selection;

class SelectionPolicy : public Policy
{
public:
    typedef shared_ptr<SelectionPolicy> Ptr;

    SelectionPolicy(const owlapi::model::IRI& iri = owlapi::model::IRI());

    virtual ~SelectionPolicy() = default;

    /**
     * Selection policies can be chained, so that apply returns an
     * updated selection
     */
    virtual Selection apply(const Selection& agentSelection,
            const OrganizationModelAsk& ask) const;

    void add(const SelectionPolicy::Ptr& policy) { mPolicyChain.push_back(policy); }

protected:
    std::vector<SelectionPolicy::Ptr> mPolicyChain;
};

} // policies
} // moreorg

#endif // ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP
