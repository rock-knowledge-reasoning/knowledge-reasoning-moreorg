#include "ActorModelLink.hpp"
#include <iostream>
#include <algorithm>
#include <assert.h>

namespace organization_model {
namespace organization_model {

ActorModelLink::ActorModelLink() {}

ActorModelLink::ActorModelLink(const EndpointModel& endpoint0, const EndpointModel& endpoint1)
{
    mEndpoints.push_back(endpoint0);
    mEndpoints.push_back(endpoint1);

    std::sort(mEndpoints.begin(), mEndpoints.end());
}

bool ActorModelLink::operator==(const ActorModelLink& other) const
{
    return other.mEndpoints == this->mEndpoints;
}

bool ActorModelLink::operator<(const ActorModelLink& other) const
{
    if(other.mEndpoints.size() < this->mEndpoints.size())
    {
        return true;
    }

    assert(other.mEndpoints.size() == 2);
    assert(this->mEndpoints.size() == 2);

    if(mEndpoints[0] < other.mEndpoints[0])
    {
        return true;
    } else if(mEndpoints[0] == other.mEndpoints[0])
    {
        if(mEndpoints[1] < other.mEndpoints[1])
        {
            return true;
        }
    }
    return false;
}

std::string ActorModelLink::toString() const
{
    std::string txt;
    if(mEndpoints.size() == 2)
    {
        txt += this->mEndpoints[0].toString();
        txt += "<->";
        txt += this->mEndpoints[1].toString();
    }
    return txt;
}

std::ostream& operator<<(std::ostream& os, const std::vector< std::vector<ActorModelLink> > modelSet)
{
    int count = 0;
    for(const std::vector<ActorModelLink>& compositeActorModel : modelSet)
    {
        os << "#" << ++count << ": ";
        for(const ActorModelLink& link : compositeActorModel)
        {
            os << link.toString() << ";";
        }
        os << std::endl;
    }
    return os;
}

} // namespace organization_model
} // namespace organization_model
