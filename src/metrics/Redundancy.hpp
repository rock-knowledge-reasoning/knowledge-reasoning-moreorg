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

typedef std::map<IRI, Metric> IRIMetricMap;


class Redundancy
{
    OrganizationModel mOrganizationModel;

    uint32_t computeOutDegree(const IRI& iri, const IRI& relation, const IRI& filter = IRI());
    uint32_t computeInDegree(const IRI& iri, const IRI& relation, const IRI& filter = IRI());

    /**
     * Compute the probability of survival for a resource with respect to a given actor
     *
     */
    double computeProbabilityOfSurvival(const IRI& resource, const IRI& actor);

public:
    Redundancy(const OrganizationModel& organization);

    IRIMetricMap compute();

};

} // end namespace metrics
} // end namespace owl_om
#endif // OWL_OM_METRICS_REDUNDANCY_HPP
