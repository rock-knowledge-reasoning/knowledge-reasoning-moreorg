#include "WeibullPDF.hpp"

namespace moreorg {
namespace metrics {
namespace pdfs {

WeibullPDF::WeibullPDF(double eta, double b, double t0)
{
    if(eta >= 0 || b >= 0)
    {
        mEta = eta;
        b_ = b;
        t0_ = t0;
    } else
    {
        throw std::invalid_argument("moreorg::metrics::pdfs::WeibullPDF: eta "
                                    "or b parameter must be greater than 0");
    }
}

double WeibullPDF::getValue(double t) const
{
    if(t < 0)
    {
        return 1.;
    }
    double value = 1 - exp(-1 * pow(((t - t0_) / mEta), b_));
    return value;
}

double WeibullPDF::getConditional(double t0, double t1) const
{
    if(t0 >= 0)
    {
        if(t1 > t0)
        {
            return 1 -
                   ((1 - getValue(t1)) /
                    (1 -
                     getValue(t0))); // I'm not to sure if this is correct tbh
        } else if(t1 == t0)
        {
            return getValue(
                t0); // use non-conditional probability of failure if t1 == t0
        }
    }
    throw std::invalid_argument("t0 and t1 must be >= 0 and t1 >= t0");
}

} // end namespace pdfs
} // end namespace metrics
} // end namespace moreorg
