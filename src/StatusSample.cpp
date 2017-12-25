#include "StatusSample.hpp"

namespace organization_model {

StatusSample::StatusSample()
{}

StatusSample::StatusSample(const Agent& instance,
            const base::Position& fromLocation,
            const base::Position& toLocation,
            size_t fromTime,
            size_t toTime,
            bool active,
            activity::Type activity,
            const FunctionalityRequirement& functionalityRequirement)
    : mAgent(instance)
    , mFromLocation(fromLocation)
    , mToLocation(toLocation)
    , mFromTime(fromTime)
    , mToTime(toTime)
    , mIsActive(active)
    , mActivityType(activity)
    , mFunctionalityRequirement(functionalityRequirement)
{
}

} // end namespace organization_model
