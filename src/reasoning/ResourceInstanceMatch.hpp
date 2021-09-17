#ifndef ORGANIZATION_MODEL_REASONING_RESOURCE_INSTANCE_MATCH_HPP
#define ORGANIZATION_MODEL_REASONING_RESOURCE_INSTANCE_MATCH_HPP

#include <gecode/set.hh>
#include <gecode/int.hh>
#include <gecode/search.hh>
#include <base-logging/Logging.hpp>

#include <owlapi/model/OWLOntology.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>

#include "../SharedPtr.hpp"
#include "../OrganizationModelAsk.hpp"
#include "../ModelPool.hpp"
#include "../vocabularies/OM.hpp"
#include "ModelBound.hpp"
#include "../ResourceInstance.hpp"

namespace moreorg {
namespace reasoning {

/**
 * ResourceInstanceMatch allows to search for a valid solution for checking cardinality constraints
 * of resources by formulation as CSP (Constraint Satisfaction Problem).
 * The CSP is defined by a query that searches to fulfill a set of cardinality
 * restrictions. Available resources are implicitly defined -- also by a list of
 * cardinality restrictions. This originates from the fact that these
 * restrictions describe a model that a certain instance ('robot') fulfills and thus
 * defines which resource have to exist for this robot.
 *
 * While in an actual instance resources might be missing solving the csp allows for model
 * checking in the first place.
 *
 \verbatim

 #include <moreorg/vocabulary/OM.hpp>

 using namespace moreorg;

 OrganizationModel::Ptr om = make_shared<OrganizationModel>("organization-model.owl");
 OrganizationModelAsk ask(om);
 ...
 IRI sherpa = vocabulary::OM::resolve("Sherpa");
 Agent agent;
 AtomicAgent aa(0, sherpa);
 agent.add(aa);

 IRI move_to = vocabulary::OM::resolve("MoveTo");

 ResourceInstance::List r_sherpa = om.getRelated(agent);
 std::vector<OWLCardinalityRestriction::Ptr> r_move_to = om.ontology().getCardinalityRestrictions(move_to);


 try {
     // Check whether 'move_to' provides a subset of restriction existing for 'sherpa'
     csp::ResourceInstanceMatch::Solution solution = cs::ResourceInstanceMatch::solve(r_move_to, r_sherpa, ontology);
     std::cout << solution->toString() << std::endl;
 } catch(const std::runtime_error& e)
 {
     std::cout << "No solution found" << std::endl;
 }

 // or in compact form
 if(csp::ResourceInstanceMatch::isSupporting(sherpa, move_to, ontology);
 {
    std::cout << "Sherpa fulfills requirements for MoveTo" << std::endl;
 }
 \endverbatim
 */
class ResourceInstanceMatch : public Gecode::Space
{
    /// Required set of models
    ModelBound::List mRequiredModelBound;
    /// Available set of resource instances
    ResourceInstance::List mAvailableResources;

    /**
     * Assignments of query resources to pool resources. This is what has to be solved.
     *                    available-service-0 available-service1
     * requirement
     * service-0                  1                   2
     * service-1                  1
     *
     * Constraints apply to min/max of sum of row --> e.g. min/max cardinality of service-0
     * Constraints apply to min/max of sum of col --> e.g. min/max cardinality of available-service-0
     * Constraints apply to individual cells due to fulfillment of service (we assume most granular resolution of services)
     */
    Gecode::IntVarArray mModelAssignment;

    ResourceInstanceMatch* solve();

protected:
    ResourceInstanceMatch(const ModelBound::List& required,
            const ResourceInstance::List& provided,
            OrganizationModelAsk ask);

    /**
     * Search support
     * This copy constructor is required for the search engine
     * and it has to provide a deep copy
     */
    ResourceInstanceMatch(ResourceInstanceMatch& s);

    /**
     * This method is called by the search engine
     */
    virtual Gecode::Space* copy(void);

public:

    typedef shared_ptr<ResourceInstanceMatch> Ptr;
    class Solution
    {
    public:
        std::string toString(uint32_t indent = 0) const;

        /**
         * Get the bound for a given model
         * \throws std::invalid_argument if model is not part of the solution
         */
        ResourceInstance::List getAssignments(const owlapi::model::IRI& model) const;

        /**
         * Set the list of resources that together fulfill the requirement of
         * the given model
         */
        void setAssignments(const ModelBound& model, const
                ResourceInstance::List& assignments) { mAssignments[model] = assignments; }

        /**
         * Add the assignment to existing assignements of resources to fulfill the requirement
         * \param model requirement model
         * \param assignment model bound of available resources that are assigned to fulfill the requirement
         */
        void addAssignment(const ModelBound& model, const ResourceInstance& assignment) { mAssignments[model].push_back(assignment); }
        /**
         * @brief Remove list of assignments from available Resource List
         * 
         * @param availableList 
         * @return ResourceInstance::List 
         */
        ResourceInstance::List removeAssignmentsFromList(ResourceInstance::List &assignmentsToRemove);

    private:
        // Assignments for this model type
        std::map<ModelBound, ResourceInstance::List> mAssignments;
    };

    ResourceInstanceMatch::Solution getSolution() const;

    void print(std::ostream& os) const;

    /**
     * Check if provider resources fulfill the model requirements
     * \return A solution that list the dedicated models being used an their
     * min/max bounds
     */
    static ResourceInstanceMatch::Solution solve(const ModelBound::List& required,
            const ResourceInstance::List& available,
            const OrganizationModelAsk& ask);

    /**
     * Create a string representation of this object
     * \return Stringified ResourceInstanceMatch object
     */
    std::string toString() const;

    /**
     * Convert the cardinality restrictions to a more compact representation,
     * where each entry is a model bound, i.e. a single object defining both min
     * and max cardinality
     * (in contrast a cardinality restriction describes only one, min or max)
     */
    static ModelBound::List toModelBoundList(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& restrictions);

    /**
     * Check if the serviceModel is supported by the providerModel
     * \param agent Agent for which the support should be validated
     * \param serviceModel
     * \param resourceMatch
     * \return True if the providerModel supports the serviceModel
     */
    static bool isSupporting(const Agent& agent, const owlapi::model::IRI& serviceModel,
            const OrganizationModelAsk& ask,
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has());


    static bool isSupporting(const ResourceInstance::List& providerRestrictions,
            const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& serviceRestrictions,
            const OrganizationModelAsk& ask);
};

} // end namespace reasoning
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_CSP_RESOURCE_INSTANCE_MATCH_HPP
