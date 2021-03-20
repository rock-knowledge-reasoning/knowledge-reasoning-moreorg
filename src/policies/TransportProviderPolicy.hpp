#ifndef ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP
#define ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP

#include "SelectionPolicy.hpp"

namespace moreorg {
namespace policies {

/**
 * Identify the transport provider that is active to
 * provide the mobility / transport service for the agent
 *
 * This policy assumes is that only one transport system can be active at a
 * time
 */
class TransportProviderPolicy : public SelectionPolicy
{
public:
    TransportProviderPolicy();

    TransportProviderPolicy(const ModelPool& modelPool, const moreorg::OrganizationModelAsk& ask);

    virtual ~TransportProviderPolicy();

    void update(const ModelPool& pool, const OrganizationModelAsk& ask);

private:
    static PolicyRegistration<TransportProviderPolicy> __attribute__((used)) msRegistration;
};

} // end namespace policies
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP
