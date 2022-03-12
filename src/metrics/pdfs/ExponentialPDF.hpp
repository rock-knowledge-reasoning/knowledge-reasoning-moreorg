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

} // end namespace distribution_functions
} // end namespace metrics
} // end namespace moreorg
