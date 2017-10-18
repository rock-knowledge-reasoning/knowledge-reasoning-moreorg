#include "ValueBound.hpp"
#include <sstream>
#include <limits>

namespace organization_model {

ValueBound::ValueBound()
    : mMin(std::numeric_limits<double>::min())
    , mMax(std::numeric_limits<double>::max())
{}

ValueBound::ValueBound(double min, double max)
    : mMin(min)
    , mMax(max)
{}

std::string ValueBound::toString() const
{
    std::stringstream ss;
    ss << "[" << mMin << "," << mMax << "]";
    return ss.str();
}

} // end organization_model
