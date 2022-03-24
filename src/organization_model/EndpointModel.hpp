#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_ENDPOINT_MODEL_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_ENDPOINT_MODEL_HPP

#include <owlapi/model/IRI.hpp>

namespace moreorg {
namespace moreorg {

/**
 * An EndpointModel describe the actor and the interface role
 */
class EndpointModel
{
    owlapi::model::IRI mActorModel;
    owlapi::model::IRI mInterfaceRole;

public:
    EndpointModel();
    EndpointModel(const owlapi::model::IRI& actorModel,
                  const owlapi::model::IRI& interfaceRole);

    bool operator<(const EndpointModel& other) const;

    bool operator==(const EndpointModel& other) const;

    std::string toString() const;
};

} // end namespace moreorg
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_ENDPOINT_MODEL_HPP
