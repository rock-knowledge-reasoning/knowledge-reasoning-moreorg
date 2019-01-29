#ifndef ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP

#include "../Policy.hpp"

namespace organization_model {
namespace policies {

/**
 *
 */
class EnergyProviderPolicy : public Policy
{
public:
    EnergyProviderPolicy();

    EnergyProviderPolicy(const ModelPool& pool, const OrganizationModelAsk& ask);

    virtual ~EnergyProviderPolicy();

    const std::map<owlapi::model::IRI, double>& getSharesByType() const { return mEnergyProviderSharesByType; }

    void update(const ModelPool& pool, const OrganizationModelAsk& ask);

private:
    std::map<owlapi::model::IRI, double> mEnergyProviderSharesByType;
};

} // end namespace policies
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_POLICIES_ENERGY_PROVIDER_POLICY_HPP
