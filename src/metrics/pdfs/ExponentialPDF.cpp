#include "ExponentialPDF.hpp"

namespace moreorg {
namespace metrics {
namespace pdfs {

ExponentialPDF::ExponentialPDF(double failureRate)
    : mFailureRate(failureRate)
{}

double ExponentialPDF::getValue(double t) const
{
    if (t < 0)
    {
        return 0;
    }

    double value = 1 - exp((-1. * mFailureRate * t));
    return value;
}

double ExponentialPDF::getConditional(double t0, double t1) const
{
    // not sure if this is the value im looking for
    if (t1 >= t0 && t0 >= 0)
    {
        double conditional = (1 - getValue(t1)) / (1 - getValue(t0));
        return conditional;
    }

    return 0;
}

} // end namespace pdfs
} // end namespace metrics
} // end namespace moreorg
