#ifndef MOREORG_METRICS_PDFS_WEIBULL_PDF_HPP
#define MOREORG_METRICS_PDFS_WEIBULL_PDF_HPP

#include "../ProbabilityDensityFunction.hpp"

namespace moreorg {
namespace metrics {
namespace pdfs {

class WeibullPDF : public ProbabilityDensityFunction
{
private:
    double mEta;
    double b_;
    double t0_;

public:
    WeibullPDF(double eta, double b, double t0 = -1);

    double getValue(double t = 0) const override;

    double getConditional(double t0 = 0, double t1 = 0) const override;
};

} // end namespace pdfs
} // end namespace metrics
} // end namespace moreorg

#endif // MOREORG_METRICS_PDFS_WEIBULL_PDF_HPP
