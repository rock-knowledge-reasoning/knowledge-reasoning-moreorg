#ifndef ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP

#include "../OrganizationModelAsk.hpp"
#include "../AtomicAgent.hpp"

namespace organization_model {
    class Agent;
}

namespace organization_model {
namespace policies {

/**
 *
 */
class EnergyProviderPolicy
{
public:
    void update(const Agent& agent, const OrganizationModelAsk& ask);

    /**
     * Get the energy consumption shared
     */
    const std::map<AtomicAgent, double>& getShares() const { return mEnergyProviderShares; }

private:
    std::map<AtomicAgent, double> mEnergyProviderShares;
};

} // end namespace policies
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP
