#ifndef OWL_OM_METRICS_REDUNDANCY_HPP
#define OWL_OM_METRICS_REDUNDANCY_HPP

#include <owl_om/OrganizationModel.hpp>

namespace owl_om {
namespace metrics {

struct ModelSurvivability
{
    owlapi::model::OWLCardinalityRestriction::Ptr restriction;
    double modelProbability;
    double redundancy;

    ModelSurvivability(owlapi::model::OWLCardinalityRestriction::Ptr restriction, double modelProbability, double redundancy)
        : restriction(restriction)
        , modelProbability(modelProbability)
        , redundancy(redundancy)
    {}

    double getProbabilityOfSurvival() const { return 1 - pow(1-modelProbability, redundancy); }

    owlapi::model::IRI getQualification() const { return restriction->getQualification(); }
    uint32_t getCardinality() const { return restriction->getCardinality(); }

    std::string toString() const 
    {
        std::stringstream ss;
        ss << " ModelSurvivability: " << std::endl;
        ss << "    modelProbability: " << modelProbability << std::endl;
        ss << "    redundancy: " << redundancy << std::endl;
        ss << "    probabilityOfSurvival: " << getProbabilityOfSurvival() << std::endl;
        ss << "  > " << restriction->toString();
        return ss.str();
    }
};

struct Metric
{
    std::string label;
    uint32_t outDegree;
    uint32_t inDegree;

    Metric()
    {}

    Metric(const std::string& label, uint32_t out, uint32_t in)
        : label(label)
        , outDegree(out)
        , inDegree(in)
    {}

    std::string toString() const 
    { 
        std::stringstream ss;
        ss << "label: " << label << ", ";
        ss << "outDegree: " << outDegree << ", ";
        ss << "inDegree: " << inDegree;
        return ss.str();
    }
};

typedef std::map<owlapi::model::IRI, Metric> IRIMetricMap;


class Redundancy
{
    OrganizationModel mOrganizationModel;
    owlapi::model::OWLOntologyAsk mAsk;

    uint32_t computeOutDegree(const owlapi::model::IRI& iri, const owlapi::model::IRI& relation, const owlapi::model::IRI& klass = owlapi::model::IRI());
    uint32_t computeInDegree(const owlapi::model::IRI& iri, const owlapi::model::IRI& relation, const owlapi::model::IRI& klass = owlapi::model::IRI());

    /**
     * Compute the probability of survival for a service/capability with respect to a given model
     *
     * It use the assigned restriction on that model to infer what is available
     * and matches this to the service requirements accordingly
     *
     */
    double computeModelBasedProbabilityOfSurvival(const owlapi::model::IRI& function, const owlapi::model::IRI& model);

    /**
     * Compute the probability of survival for the function of the system with
     * respect to a given set of composite actor model -- which is defined by a set of atomic
     * actor models
     */
    double computeModelBasedProbabilityOfSurvival(const owlapi::model::IRI& function, const std::map<owlapi::model::IRI,uint32_t>& models);

public:
    double compute(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required, const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& available);

    Redundancy(const OrganizationModel& organization);

    IRIMetricMap compute();

};

} // end namespace metrics
} // end namespace owl_om
#endif // OWL_OM_METRICS_REDUNDANCY_HPP
