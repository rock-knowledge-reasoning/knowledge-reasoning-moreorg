#ifndef ORGANIZATION_MODEL_VALUE_BOUND_HPP
#define ORGANIZATION_MODEL_VALUE_BOUND_HPP

#include <string>

namespace organization_model {

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

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_VALUE_BOUND_HPP
