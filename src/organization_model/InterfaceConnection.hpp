#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_INTERFACE_CONNECTION_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_INTERFACE_CONNECTION_HPP

#include <owlapi/model/IRI.hpp>
#include <moreorg/organization_model/ActorModelLink.hpp>

namespace moreorg {
namespace moreorg {

struct InterfaceConnection
{
    InterfaceConnection();
    InterfaceConnection(const owlapi::model::IRI& interface0, const owlapi::model::IRI& interface1);

    owlapi::model::IRI begin;
    owlapi::model::IRI end;

    // The ActorModel pair this interface connection can be abstracted to
    ActorModelLink actorModelLink;

    owlapi::model::IRIList parents;
    owlapi::model::IRIList modelParents;

    void setActorModelLink(const ActorModelLink& link) { actorModelLink = link; }

    void addParent(const owlapi::model::IRI& parent);
    bool sameParents(const InterfaceConnection& other) const;

    bool selfReferencing() const;

    /**
     * Test if the two interface connections use the same interface
     */
    bool useSameInterface(const InterfaceConnection& other) const;

    bool operator<(const InterfaceConnection& other) const;
    bool operator==(const InterfaceConnection& other) const;

    std::string toString() const;
};

typedef std::vector< InterfaceConnection > InterfaceConnectionList;
typedef std::vector< InterfaceConnectionList > InterfaceCombinationList;

std::ostream& operator<<(std::ostream& os, const InterfaceConnection& connection);
std::ostream& operator<<(std::ostream& os, const InterfaceConnectionList& list);
std::ostream& operator<<(std::ostream& os, const InterfaceCombinationList& list);

} // end namespace moreorg
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_INTERFACE_CONNECTION_HPP
