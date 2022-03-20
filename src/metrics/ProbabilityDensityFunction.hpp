#ifndef MOREORG_METRICS_PROBABILITY_DENSITY_FUNCTION_HPP
#define MOREORG_METRICS_PROBABILITY_DENSITY_FUNCTION_HPP

#include <math.h>
#include <stdexcept>
#include "../SharedPtr.hpp"
#include "../OrganizationModelAsk.hpp"

namespace moreorg {
namespace metrics {

class ProbabilityDensityFunction 
{

    public:
        typedef shared_ptr<ProbabilityDensityFunction> Ptr;

        virtual ~ProbabilityDensityFunction() = default;

        virtual double getValue(double t = 0) const = 0;  

        virtual double getConditional(double t0 = 0, double t1 = 0) const = 0;

        static Ptr getInstance(const OrganizationModelAsk& organizationModelAsk,
                               const owlapi::model::IRI& qualification);
};

} // namespace metrics
} // namespace moreorg

#endif // MOREORG_METRICS_PROBABILITY_DENSITY_FUNCTION_HPP
