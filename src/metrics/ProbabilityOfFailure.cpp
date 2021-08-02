#include "ProbabilityOfFailure.hpp"

namespace moreorg {
namespace metrics {

template <class T>
ProbabilityOfFailure<T>::ProbabilityOfFailure(const owlapi::model::OWLObjectCardinalityRestriction::Ptr& restriction,
            T& resourcePoFDistribution,
            double redundancy)
    : mObjectRestriction(restriction)
    , mRedundancy(redundancy)
{
  mModelProbabilityOfFailureDistribution = resourcePoFDistribution;
}
template <class T>
double ProbabilityOfFailure<T>::getProbabilityOfFailure(double time) const
{
    double pos = 1 - mModelProbabilityOfFailureDistribution.getValue(time);
    double pSerialSystem = pow(pos, getCardinality());
    return pow(1-pSerialSystem, mRedundancy);
}
template <class T>
double ProbabilityOfFailure<T>::getProbabilityOfFailureDensity(double time_start, double time_end) const
{
    double pos = mModelProbabilityOfFailureDistribution.getDensity(time_start, time_end);
    double pSerialSystem = pow(pos, getCardinality());
    return pow(1-pSerialSystem, mRedundancy);
}
template <class T>
owlapi::model::IRI ProbabilityOfFailure<T>::getQualification() const
{
    return mObjectRestriction->getQualification();
}
template <class T>
uint32_t ProbabilityOfFailure<T>::getCardinality() const
{
    return mObjectRestriction->getCardinality();
}
template <class T>
std::string ProbabilityOfFailure<T>::toString() const
{
    std::stringstream ss;
    ss << " ProbabilityOfFailure: " << std::endl;
    ss << "    modelProbability:      " << mModelProbabilityOfFailureDistribution.getValue() << std::endl;
    ss << "    redundancy:            " << mRedundancy << std::endl;
    ss << "    probabilityOfFailure: " << getProbabilityOfFailure() << std::endl;
    ss << "  > " << mObjectRestriction->toString();
    return ss.str();
}

} // end namespace metrics
} // end namespace moreorg
