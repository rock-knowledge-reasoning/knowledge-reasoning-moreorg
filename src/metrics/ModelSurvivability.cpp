#include "ModelSurvivability.hpp"

namespace organization_model {
namespace metrics {

ModelSurvivability::ModelSurvivability(const owlapi::model::OWLCardinalityRestriction::Ptr& restriction, double modelProbability, double redundancy)
    : mRestriction(restriction)
    , mModelProbabilityOfSurvival(modelProbability)
    , mRedundancy(redundancy)
{}

double ModelSurvivability::getProbabilityOfSurvival() const
{
    double pSerialSystem = pow(mModelProbabilityOfSurvival, getCardinality());
    return 1 - pow(1-pSerialSystem, mRedundancy);
}

owlapi::model::IRI ModelSurvivability::getQualification() const
{
    return mRestriction->getQualification();
}

uint32_t ModelSurvivability::getCardinality() const
{
    return mRestriction->getCardinality();
}

std::string ModelSurvivability::toString() const
{
    std::stringstream ss;
    ss << " ModelSurvivability: " << std::endl;
    ss << "    modelProbability:      " << mModelProbabilityOfSurvival << std::endl;
    ss << "    redundancy:            " << mRedundancy << std::endl;
    ss << "    probabilityOfSurvival: " << getProbabilityOfSurvival() << std::endl;
    ss << "  > " << mRestriction->toString();
    return ss.str();
}

} // end namespace metrics
} // end namespace organization_model
