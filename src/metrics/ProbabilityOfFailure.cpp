#include "ProbabilityOfFailure.hpp"

namespace moreorg {
namespace metrics {


ProbabilityOfFailure::ProbabilityOfFailure(const owlapi::model::OWLObjectCardinalityRestriction::Ptr& restriction,
            const ProbabilityDensityFunction::Ptr& resourcePoFDistribution,
            double redundancy)
    : mObjectRestriction(restriction)
    , mRedundancy(redundancy)
{
  mModelProbabilityOfFailureDistribution = resourcePoFDistribution;
}

double ProbabilityOfFailure::getProbabilityOfFailure(double time) const
{
    double pos = 1 - mModelProbabilityOfFailureDistribution->getValue(time);
    double pSerialSystem = pow(pos, getCardinality());
    return pow(1-pSerialSystem, mRedundancy);
}


double ProbabilityOfFailure::getProbabilityOfSurvival(double time) const
{
    return 1 - getProbabilityOfFailure(time);
}

double ProbabilityOfFailure::getProbabilityOfFailureConditional(double time_start, double time_end) const
{
    double pos = mModelProbabilityOfFailureDistribution->getConditional(time_start, time_end);
    double pSerialSystem = pow(pos, getCardinality());
    return pow(1-pSerialSystem, mRedundancy);
}

double ProbabilityOfFailure::getProbabilityOfSurvivalConditional(double time_start, double time_end) const
{
    return 1 - getProbabilityOfFailureConditional(time_start, time_end);
}

owlapi::model::IRI ProbabilityOfFailure::getQualification() const
{
    return mObjectRestriction->getQualification();
}

uint32_t ProbabilityOfFailure::getCardinality() const
{
    return mObjectRestriction->getCardinality();
}

std::string ProbabilityOfFailure::toString() const
{
    std::stringstream ss;
    ss << " ProbabilityOfFailure: " << std::endl;
    ss << "    modelProbability:      " << mModelProbabilityOfFailureDistribution->getValue() << std::endl;
    ss << "    redundancy:            " << mRedundancy << std::endl;
    ss << "    probabilityOfFailure: " << getProbabilityOfFailure() << std::endl;
    ss << "  > " << mObjectRestriction->toString();
    return ss.str();
}

} // end namespace metrics
} // end namespace moreorg
