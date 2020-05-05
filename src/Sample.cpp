#include "Sample.hpp"

namespace moreorg {

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

} // end namespace moreorg
