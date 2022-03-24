#ifndef ORGANIZATION_MODEL_POLICIES_DISTRIBUTION_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_DISTRIBUTION_POLICY_HPP

#include "../InferenceRule.hpp"
#include "../Policy.hpp"

namespace moreorg {
namespace policies {

struct Distribution
{
    std::map<owlapi::model::IRI, double> shares;
};

/**
 * Default interface for a Policy that assigns a particular
 * share to subsystems of an agent
 */
class DistributionPolicy : public Policy
{
public:
    using Ptr = shared_ptr<DistributionPolicy>;

    DistributionPolicy() = default;

    DistributionPolicy(const owlapi::model::IRI& property,
                       const OrganizationModelAsk& ask);

    virtual ~DistributionPolicy() = default;

    virtual Distribution apply(const ModelPool& modelPool,
                               const OrganizationModelAsk& ask) const;

protected:
    OrganizationModelAsk mAsk;

    InferenceRule::Ptr mAtomicAgentRule;
    InferenceRule::Ptr mCompositeAgentRule;
};

} // namespace policies
} // namespace moreorg

#endif // ORGANIZATION_MODEL_POLICIES_DISTRIBUTION_POLICY_HPP
