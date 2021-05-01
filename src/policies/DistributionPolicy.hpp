#ifndef ORGANIZATION_MODEL_POLICIES_DISTRIBUTION_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_DISTRIBUTION_POLICY_HPP

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
    typedef shared_ptr<DistributionPolicy> Ptr;

    DistributionPolicy(const owlapi::model::IRI& iri = owlapi::model::IRI());

    virtual ~DistributionPolicy() = default;

    virtual Distribution apply(const ModelPool& modelPool,
            const OrganizationModelAsk& ask) const = 0;
};

} // policies
} // moreorg

#endif // ORGANIZATION_MODEL_POLICIES_DISTRIBUTION_POLICY_HPP
