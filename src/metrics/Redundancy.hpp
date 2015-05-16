#ifndef ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP
#define ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP

#include <organization_model/OrganizationModel.hpp>
#include <owlapi/model/OWLCardinalityRestriction.hpp>

namespace organization_model {
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
    boost::shared_ptr<owlapi::model::OWLOntologyAsk> mpAsk;

public:
    Redundancy(const OrganizationModel& organization);

    IRISurvivabilityMap compute();

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


    double compute(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& required, const std::vector< std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> >& available);

    static std::string toString(const IRISurvivabilityMap& map);
};

} // end namespace metrics
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_METRICS_REDUNDANCY_HPP
