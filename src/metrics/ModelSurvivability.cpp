#include "ModelSurvivability.hpp"

namespace organization_model {
namespace metrics {

ModelSurvivability::ModelSurvivability(owlapi::model::OWLCardinalityRestriction::Ptr restriction, double modelProbability, double redundancy)
    : restriction(restriction)
    , modelProbability(modelProbability)
    , redundancy(redundancy)
{}

double ModelSurvivability::getProbabilityOfSurvival() const
{
    return 1 - pow(1-modelProbability, redundancy);
}

owlapi::model::IRI ModelSurvivability::getQualification() const
{
    return restriction->getQualification();
}

uint32_t ModelSurvivability::getCardinality() const 
{ 
    return restriction->getCardinality();
}

std::string ModelSurvivability::toString() const 
{
    std::stringstream ss;
    ss << " ModelSurvivability: " << std::endl;
    ss << "    modelProbability: " << modelProbability << std::endl;
    ss << "    redundancy: " << redundancy << std::endl;
    ss << "    probabilityOfSurvival: " << getProbabilityOfSurvival() << std::endl;
    ss << "  > " << restriction->toString();
    return ss.str();
}

} // end namespace metrics
} // end namespace organization_model
