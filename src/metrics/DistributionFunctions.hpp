#ifndef DISTRIBUTION_FUNCTIONS_HPP
#define DISTRIBUTION_FUNCTIONS_HPP

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
    virtual double getValue (double t = 0) const = 0;  
    virtual double getConditional (double t0 = 0, double t1 = 0) const = 0;
    static Ptr getInstance(const OrganizationModelAsk& organizationModelAsk, const owlapi::model::IRI& qualification);

};

class WeibullPDF : public ProbabilityDensityFunction
{
  private:
    double eta_;
    double b_;
    double t0_;

    public:
      WeibullPDF(double eta, double b, double t0 = -1)
        {
          if (eta >= 0 || b >= 0)
          {
            eta_ = eta;
            b_ = b;
            t0_ = t0;
          } else
            throw std::invalid_argument("Weibull::Weibull : eta or b parameter must be greater than 0");
          
        } 

      double getValue (double t = 0) const override
      {
        if (t < 0)
        {
            return 1.;
        }
        double value = 1 - exp(-1 * pow(((t - t0_) / eta_), b_));
        return value;
      }

      double getConditional (double t0 = 0, double t1 = 0) const override
      {
        if (t0 >= 0)
        {
          if (t1 > t0)
          {
              return 1 - ((1 - getValue(t1)) / (1 - getValue(t0))); // I'm not to sure if this is correct tbh
          } else if (t1 == t0)
          {
              return getValue(t0); // use non-conditional probability of failure if t1 == t0
          }
          }
        throw std::invalid_argument("t0 and t1 must be >= 0 and t1 >= t0");
      }
};

class ExponentialPDF : public ProbabilityDensityFunction
{
  private:
    double mFailureRate;
  public:
    ExponentialPDF(double failureRate):
    mFailureRate(failureRate)
    {}

    double getValue(double t = 0) const override
    {
      if (t < 0) return 0;
      double value = 1 - exp((-1. * mFailureRate * t));
      return value;
    }

    double getConditional(double t0 = 0, double t1 = 0) const override
    {
      // not sure if this is the value im looking for
      if (t1 >= t0 && t0 >= 0)
      {
        double conditional = (1 - getValue(t1)) / (1 - getValue(t0));
        return conditional;
      } else return 0;

    }
};

class ConstantPDF : public ProbabilityDensityFunction
{
  private:
    double mProbabilityOfFailure;
  public:
  ConstantPDF(double probabilityOfFailure):
  mProbabilityOfFailure(probabilityOfFailure)
  {}
  double getValue(double t = 0) const override
  {
    if (t < 0) return 0;
    return mProbabilityOfFailure;
  }
  double getConditional (double t0 = 0, double t1 = 0) const override
  {
    if (t1 >= t0 && t0 >= 0)
      {
        return mProbabilityOfFailure;
      } else return 0;
  }
};

} // namespace metrics
} // namespace moreorg

#endif // DISTRIBUTION_FUNCTIONS_HPP