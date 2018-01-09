#ifndef ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP
#define ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP

#include "../AtomicAgent.hpp"
#include "../OrganizationModelAsk.hpp"

namespace organization_model {
    class Agent;
}

namespace organization_model {
namespace policies {

class TransportProviderPolicy
{
public:
    /**
     * Identify the transport providers that are active to
     * provide the mobility / transport service for the agent
     */
    const AtomicAgent::Set& activeTransportProviders(const Agent& agent,
            const organization_model::OrganizationModelAsk& ask) const;

private:
    mutable std::map<Agent, AtomicAgent::Set> mActiveTransportProviders;
};

} // end namespace policies
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_POLICIES_TRANSPORT_PROVIDER_HPP
