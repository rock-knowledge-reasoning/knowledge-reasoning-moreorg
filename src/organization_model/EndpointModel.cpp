#include <owl_om/organization_model/EndpointModel.hpp>
#include <sstream>

using namespace owlapi::model;

namespace owl_om {
namespace organization_model {

EndpointModel::EndpointModel()
{}

EndpointModel::EndpointModel(const IRI& actorModel, const IRI& interfaceRole)
    : mActorModel(actorModel)
    , mInterfaceRole(interfaceRole)
{}

bool EndpointModel::operator<(const EndpointModel& other) const
{
    if(mActorModel < other.mActorModel)
    {
        return true;
    } else if(mActorModel == other.mActorModel)
    {
        if(mInterfaceRole < other.mInterfaceRole)
        {
            return true;
        }
    }

    return false;
}

bool EndpointModel::operator==(const EndpointModel& other) const
{
    return this->mActorModel == other.mActorModel && this->mInterfaceRole == other.mInterfaceRole;
}

std::string EndpointModel::toString() const
{
    std::stringstream ss;
    ss << "[";
    if(!mActorModel.empty())
    {
        ss << mActorModel.getFragment();
    }
    ss << ":";
    if(!mInterfaceRole.empty())
    {
        ss << mInterfaceRole.getFragment();
    }
    ss << "]";
    return ss.str();
}

} // end namespace organization_model
} // end namespace owl_om
