#include "ProbabilityOfFailure.hpp"
#include <stdexcept>

namespace moreorg {
namespace metrics {


ProbabilityOfFailure::ProbabilityOfFailure(const owlapi::model::OWLObjectCardinalityRestriction::Ptr& restriction,
            const ProbabilityDensityFunction::Ptr& resourcePoFDistribution,
            double redundancy)
    : mObjectRestriction(restriction)
    , mModelProbabilityOfFailureDistribution(resourcePoFDistribution)
    , mRedundancy(redundancy)
{
    if (!mModelProbabilityOfFailureDistribution)
    {
        throw std::invalid_argument("moreorg::metrics::ProbabilityOfFailure no model provided!");
    }
}

ProbabilityOfFailure::ProbabilityOfFailure(const owlapi::model::OWLObjectCardinalityRestriction::Ptr& restriction,
            const ProbabilityDensityFunction::Ptr& resourcePoFDistribution,
            double redundancy,
            reasoning::ModelBound::List modelBoundAssignments)
    : mObjectRestriction(restriction)
    , mModelProbabilityOfFailureDistribution(resourcePoFDistribution)
    , mRedundancy(redundancy)
{
    if (!mModelProbabilityOfFailureDistribution)
    {
        throw std::invalid_argument("moreorg::metrics::ProbabilityOfFailure no model provided!");
    }
    mModelBoundAssignments = modelBoundAssignments;
}

double ProbabilityOfFailure::getProbabilityOfFailure(double time) const
{
    double pos = 1 - mModelProbabilityOfFailureDistribution->getValue(time);
    double pSerialSystem = pow(pos, getCardinality());
    return 1 - pSerialSystem;
}


double ProbabilityOfFailure::getProbabilityOfSurvival(double time) const
{
    return 1 - getProbabilityOfFailure(time);
}

double ProbabilityOfFailure::getProbabilityOfFailureConditional(double time_start, double time_end) const
{
    double pos = 1 - mModelProbabilityOfFailureDistribution->getConditional(time_start, time_end);
    double pSerialSystem = pow(pos, getCardinality());
    return 1 - pSerialSystem;
}

double ProbabilityOfFailure::getProbabilityOfSurvivalConditional(double time_start, double time_end) const
{
    return 1 - getProbabilityOfFailureConditional(time_start, time_end);
}

double ProbabilityOfFailure::getProbabilityOfFailureWithRedundancy(double time) const
{
    double pos = 1 - mModelProbabilityOfFailureDistribution->getValue(time);
    double pSerialSystem = pow(pos, getCardinality());
    return pow(1-pSerialSystem, mRedundancy);
}


double ProbabilityOfFailure::getProbabilityOfSurvivalWithRedundancy(double time) const
{
    return 1 - getProbabilityOfFailureWithRedundancy(time);
}

double ProbabilityOfFailure::getProbabilityOfFailureConditionalWithRedundancy(double time_start, double time_end) const
{
    double pos = 1 - mModelProbabilityOfFailureDistribution->getConditional(time_start, time_end);
    double pSerialSystem = pow(pos, getCardinality());
    return pow(1-pSerialSystem, mRedundancy);
}

double ProbabilityOfFailure::getProbabilityOfSurvivalConditionalWithRedundancy(double time_start, double time_end) const
{
    return 1 - getProbabilityOfFailureConditionalWithRedundancy(time_start, time_end);
}

ProbabilityDensityFunction::Ptr ProbabilityOfFailure::getProbabilityDensityFunction() const
{
    return mModelProbabilityOfFailureDistribution;
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
    ss << "    probabilityOfFailure: " << getProbabilityOfFailureWithRedundancy() << std::endl;
    ss << "  > " << mObjectRestriction->toString();
    return ss.str();
}

} // end namespace metrics
} // end namespace moreorg
