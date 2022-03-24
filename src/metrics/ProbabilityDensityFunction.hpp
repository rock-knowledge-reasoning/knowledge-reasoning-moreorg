#ifndef MOREORG_METRICS_PROBABILITY_DENSITY_FUNCTION_HPP
#define MOREORG_METRICS_PROBABILITY_DENSITY_FUNCTION_HPP

#include "../OrganizationModelAsk.hpp"
#include "../SharedPtr.hpp"
#include <math.h>
#include <stdexcept>

namespace moreorg {
namespace metrics {

class ProbabilityDensityFunction
{

public:
    using Ptr = shared_ptr<ProbabilityDensityFunction>;

    virtual ~ProbabilityDensityFunction() = default;

    virtual double getValue(double t = 0) const = 0;

    virtual double getConditional(double t0 = 0, double t1 = 0) const = 0;

    static Ptr getInstance(const OrganizationModelAsk& organizationModelAsk,
                           const owlapi::model::IRI& qualification);
};

} // namespace metrics
} // namespace moreorg

#endif // MOREORG_METRICS_PROBABILITY_DENSITY_FUNCTION_HPP
