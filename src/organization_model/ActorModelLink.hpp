#ifndef OWL_OM_ORGANIZATION_MODEL_ACTOR_MODEL_LINK_HPP
#define OWL_OM_ORGANIZATION_MODEL_ACTOR_MODEL_LINK_HPP

#include <owl_om/organization_model/EndpointModel.hpp>

namespace owl_om {
namespace organization_model {

/**
 * An ActorModelLink describes the combination of two actors via 
 * a unique tuple of (electro-mechanical) interfaces
 */
class ActorModelLink
{
    // Mhm, how to map from instance back to the model,
    // i.e. which roles are fulfilled by which interface
    // -- is that truely relevant, not until we have more detailled contraints,
    // i.e. *a* mapping would be sufficient for now
    std::vector<EndpointModel> mEndpoints;

public:
    ActorModelLink();

    /**
     * Construct an ActorModelLink from two Endpoints
     */
    ActorModelLink(const EndpointModel& endpoint0, const EndpointModel& endpoint1);

    bool operator==(const ActorModelLink& other) const;
    bool operator<(const ActorModelLink& other) const;
    std::string toString() const;
};

std::ostream& operator<<(std::ostream& os, const std::vector< std::vector<ActorModelLink> > modelSet);

} // namespace organization_model
} // namespace owl_om

#endif // OWL_OM_ORGANIZATION_MODEL_ACTOR_MODEL_LINK_HPP
