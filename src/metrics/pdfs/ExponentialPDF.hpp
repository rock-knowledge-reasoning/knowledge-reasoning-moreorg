#ifndef MOREORG_METRICS_PDFS_EXPONENTIAL_PDF_HPP
#define MOREORG_METRICS_PDFS_EXPONENTIAL_PDF_HPP

#include "../ProbabilityDensityFunction.hpp"

namespace moreorg {
namespace metrics {
namespace pdfs {

class ExponentialPDF : public ProbabilityDensityFunction
{
private:
    double mFailureRate;

public:
    ExponentialPDF(double failureRate);

    double getValue(double t = 0) const override;

    double getConditional(double t0 = 0, double t1 = 0) const override;
};

} // namespace pdfs
} // end namespace metrics
} // end namespace moreorg

#endif // MOREORG_METRICS_PDFS_EXPONENTIAL_PDF_HPP
