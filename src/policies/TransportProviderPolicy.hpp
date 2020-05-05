#ifndef ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP
#define ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP

#include "../Policy.hpp"
#include "../OrganizationModelAsk.hpp"

namespace moreorg {
namespace policies {

class TransportProviderPolicy : public Policy
{
public:

    TransportProviderPolicy();

    TransportProviderPolicy(const ModelPool& modelPool, const moreorg::OrganizationModelAsk& ask);

    virtual ~TransportProviderPolicy();

    void update(const ModelPool& pool, const OrganizationModelAsk& ask);

    /**
     * Identify the transport providers that are active to
     * provide the mobility / transport service for the agent
     *
     * This policy assumes is that only one transport system can be active at a
     * time
     */
    const ModelPool& getActiveTransportProviders() const { return mActiveTransportProviders; }

private:
    ModelPool mActiveTransportProviders;
};

} // end namespace policies
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP
