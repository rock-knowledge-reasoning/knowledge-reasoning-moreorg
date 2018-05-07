#include "Sample.hpp"

namespace organization_model {

Sample::Sample(const base::Position& fromLocation,
           const base::Position& toLocation,
           size_t fromTime,
           size_t toTime
    )
    : mFromLocation(fromLocation)
    , mToLocation(toLocation)
    , mFromTime(fromTime)
    , mToTime(toTime)
{
}

Sample::~Sample()
{}

} // end namespace organization_model
