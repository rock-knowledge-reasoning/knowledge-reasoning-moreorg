#include "StatusSample.hpp"

namespace organization_model {

StatusSample::StatusSample()
{}

StatusSample::StatusSample(const Agent& instance,
            const base::Position& fromLocation,
            const base::Position& toLocation,
            size_t fromTime,
            size_t toTime,
            Agent::OperationalStatus operationalStatus,
            activity::Type activity,
            const Resource::Set& resourceRequirements)
    : mAgent(instance)
    , mFromLocation(fromLocation)
    , mToLocation(toLocation)
    , mFromTime(fromTime)
    , mToTime(toTime)
    , mOperationalStatus(operationalStatus)
    , mActivityType(activity)
    , mResourceRequirements(resourceRequirements)
{
}

} // end namespace organization_model
