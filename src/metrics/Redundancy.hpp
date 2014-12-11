#ifndef OWL_OM_METRICS_REDUNDANCY_HPP
#define OWL_OM_METRICS_REDUNDANCY_HPP

#include <owl_om/OrganizationModel.hpp>

namespace owl_om {
namespace metrics {

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

    uint32_t computeOutDegree(const owlapi::model::IRI& iri, const owlapi::model::IRI& relation, const owlapi::model::IRI& filter = owlapi::model::IRI());
    uint32_t computeInDegree(const owlapi::model::IRI& iri, const owlapi::model::IRI& relation, const owlapi::model::IRI& filter = owlapi::model::IRI());

    /**
     * Compute the probability of survival for a resource with respect to a given actor
     *
     */
    double computeProbabilityOfSurvival(const owlapi::model::IRI& resource, const owlapi::model::IRI& actor);

public:
    Redundancy(const OrganizationModel& organization);

    IRIMetricMap compute();

};

} // end namespace metrics
} // end namespace owl_om
#endif // OWL_OM_METRICS_REDUNDANCY_HPP
