#ifndef OWL_OM_METRICS_REDUNDANCY_HPP
#define OWL_OM_METRICS_REDUNDANCY_HPP

#include <owl_om/OrganizationModel.hpp>

namespace owl_om {
namespace metrics {

typedef owlapi::model::IRI ServiceIRI;
typedef owlapi::model::IRI ActorIRI;
typedef std::map< std::pair<ServiceIRI, ActorIRI>, double> IRISurvivabilityMap;

/**
 * Compute redundancy for a model
 */
class Redundancy
{
    OrganizationModel mOrganizationModel;
    owlapi::model::OWLOntologyAsk mAsk;

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
    Redundancy(const OrganizationModel& organization);

    IRISurvivabilityMap compute();

    double compute(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required, const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& available);

    static std::string toString(const IRISurvivabilityMap& map);
};

} // end namespace metrics
} // end namespace owl_om
#endif // OWL_OM_METRICS_REDUNDANCY_HPP
