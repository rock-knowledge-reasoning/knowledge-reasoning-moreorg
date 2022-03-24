#include "StatusSample.hpp"

namespace moreorg {

StatusSample::~StatusSample() {}

StatusSample::StatusSample(const Agent& instance,
                           const base::Position& fromLocation,
                           const base::Position& toLocation,
                           size_t fromTime,
                           size_t toTime,
                           Agent::OperationalStatus operationalStatus,
                           activity::Type activity)
    : Sample(fromLocation, toLocation, fromTime, toTime)
    , mAgent(instance)
    , mOperationalStatus(operationalStatus)
    , mActivityType(activity)
{
}

} // end namespace moreorg
