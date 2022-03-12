#include "ConstantPDF.hpp"

namespace moreorg {
namespace metrics {
namespace pdfs {

ConstantPDF::ConstantPDF(double probabilityOfFailure)
    : mProbabilityOfFailure(probabilityOfFailure)
{}

double ConstantPDF::getValue(double t) const
{
    if (t < 0)
    {
        return 0;
    }
    return mProbabilityOfFailure;
}
double ConstantPDF::getConditional(double t0, double t1) const
{
    if (t1 >= t0 && t0 >= 0)
    {
        return mProbabilityOfFailure;
    }

    return 0;
}

} // end namespace pdfs
} // end namespace metrics
} // end namespace moreorg
