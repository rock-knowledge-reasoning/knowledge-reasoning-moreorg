#ifndef DISTRIBUTION_FUNCTIONS_HPP
#define DISTRIBUTION_FUNCTIONS_HPP

#include <math.h>
#include <stdexcept>

namespace moreorg {
namespace metrics {

class Weibull {
  private:
    double a_;
    double b_;

    public:
      Weibull(double a, double b)
        {
          if (a >= 0 || b >= 0)
          {
            a_ = a;
            b_ = b;
          } else
            throw std::invalid_argument("Weibull::Weibull : a or b parameter must be greater than 0");
          
        } 

      double getValue (double t = 0) const
      {
        if (t < 0) return 0;
        double value = 1 - exp(-1 * a_ * pow(t, b_));
        return value;
      }

      double getDensity (double t0, double t1) const
      {
        if (t1 >= t0 && t0 >= 0)
        {
          double density = (1 - getValue(t1)) / (1 - getValue(t0)); // I'm not to sure if this is correct tbh
          return density;
        } else
        return 0.;
      }
};

class Exponential {
  private:
    double mFailureRate;
  public:
    Exponential(double failureRate):
    mFailureRate(failureRate)
    {}

    double getValue(double t) const
    {
      if (t < 0) return 0;
      double value = 1 - exp((-1. * mFailureRate * t));
      return value;
    }

    double getDensity(double t0, double t1) const
    {
      // not sure if this is the value im looking for
      if (t1 >= t0 && t0 >= 0)
      {
        double density = (1 - getValue(t1)) / (1 - getValue(t0));
        return density;
      } else return 0;

    }
};

class Const {
  private:
    double mProbabilityOfFailure;
  public:
  Const(double probabilityOfFailure):
  mProbabilityOfFailure(probabilityOfFailure)
  {}
  double getValue(double t) const
  {
    if (t < 0) return 0;
    return mProbabilityOfFailure;
  }
  double getDensity (double t0, double t1) const
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