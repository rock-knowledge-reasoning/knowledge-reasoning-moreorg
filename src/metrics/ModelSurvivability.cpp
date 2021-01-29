#include "ModelSurvivability.hpp"

namespace moreorg {
namespace metrics {

ModelSurvivability::ModelSurvivability(const owlapi::model::OWLObjectCardinalityRestriction::Ptr& restriction,
        double modelProbability,
        double redundancy)
    : mObjectRestriction(restriction)
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
    return mObjectRestriction->getQualification();
}

uint32_t ModelSurvivability::getCardinality() const
{
    return mObjectRestriction->getCardinality();
}

std::string ModelSurvivability::toString() const
{
    std::stringstream ss;
    ss << " ModelSurvivability: " << std::endl;
    ss << "    modelProbability:      " << mModelProbabilityOfSurvival << std::endl;
    ss << "    redundancy:            " << mRedundancy << std::endl;
    ss << "    probabilityOfSurvival: " << getProbabilityOfSurvival() << std::endl;
    ss << "  > " << mObjectRestriction->toString();
    return ss.str();
}

} // end namespace metrics
} // end namespace moreorg
