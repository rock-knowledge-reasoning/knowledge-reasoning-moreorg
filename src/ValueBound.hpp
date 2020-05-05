#ifndef ORGANIZATION_MODEL_VALUE_BOUND_HPP
#define ORGANIZATION_MODEL_VALUE_BOUND_HPP

#include <string>

namespace moreorg {

/**
 * Allow a constrained value
 */
class ValueBound
{
public:
    ValueBound();
    ValueBound(double mMin, double mMax);

    void setMin(double v) { mMin = v; }
    void setMax(double v) { mMax = v; }

    double getMin() const { return mMin; }
    double getMax() const { return mMax; }

    std::string toString() const;

private:
    double mMin;
    double mMax;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_VALUE_BOUND_HPP
