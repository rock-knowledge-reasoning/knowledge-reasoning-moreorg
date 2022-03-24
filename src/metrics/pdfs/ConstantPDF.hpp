#ifndef MOREORG_METRICS_PDFS_CONSTANT_PDF_HPP
#define MOREORG_METRICS_PDFS_CONSTANT_PDF_HPP

#include "../ProbabilityDensityFunction.hpp"

namespace moreorg {
namespace metrics {
namespace pdfs {

class ConstantPDF : public ProbabilityDensityFunction
{
private:
    double mProbabilityOfFailure;

public:
    ConstantPDF(double probabilityOfFailure);

    double getValue(double t = 0) const override;

    double getConditional(double t0 = 0, double t1 = 0) const override;
};

} // end namespace pdfs
} // end namespace metrics
} // end namespace moreorg

#endif // MOREORG_METRICS_PDFS_CONSTANT_PDF_HPP
