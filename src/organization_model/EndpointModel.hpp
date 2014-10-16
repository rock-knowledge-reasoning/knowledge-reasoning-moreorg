#ifndef OWL_OM_ORGANIZATION_MODEL_ENDPOINT_MODEL_HPP
#define OWL_OM_ORGANIZATION_MODEL_ENDPOINT_MODEL_HPP

#include <owl_om/Ontology.hpp>

namespace owl_om {
namespace organization_model {

/**
 * An EndpointModel describe the actor and the interface role
 */
class EndpointModel
{
    IRI mActorModel;
    IRI mInterfaceRole;

public:
    EndpointModel();
    EndpointModel(const IRI& actorModel, const IRI& interfaceRole);

    bool operator<(const EndpointModel& other) const;

    bool operator==(const EndpointModel& other) const;

    std::string toString() const;
};

} // end namespace organization_model
} // end namespace owl_om
#endif // OWL_OM_ORGANIZATION_MODEL_ENDPOINT_MODEL_HPP

