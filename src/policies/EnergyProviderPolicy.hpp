#ifndef ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP

#include "DistributionPolicy.hpp"

namespace moreorg {
namespace policies {

/**
 *
 */
class EnergyProviderPolicy : public DistributionPolicy
{
public:
    EnergyProviderPolicy();

    EnergyProviderPolicy(const ModelPool& pool, const OrganizationModelAsk& ask);

    virtual ~EnergyProviderPolicy();

    void update(const ModelPool& pool, const OrganizationModelAsk& ask);

private:
    static PolicyRegistration<EnergyProviderPolicy> __attribute__((used)) msRegistration;
};

} // end namespace policies
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP
