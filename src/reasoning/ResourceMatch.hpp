#ifndef ORGANIZATION_MODEL_REASONING_RESOURCE_MATCH_HPP
#define ORGANIZATION_MODEL_REASONING_RESOURCE_MATCH_HPP

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

namespace moreorg {
namespace reasoning {

typedef std::map<owlapi::model::IRI, std::vector<int> > TypeInstanceMap;
typedef std::map<owlapi::model::IRI, owlapi::model::IRIList > AllowedTypesMap;
typedef owlapi::model::IRIList InstanceList;
typedef owlapi::model::IRIList TypeList;

/**
 * ResourceMatch allows to search for a valid solution for checking cardinality constraints
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

 using namespace owlapi;

 OWLOntology::Ptr ontology = io::OWLOntologyIO::fromFile("organization-model.owl");
 ...
 IRI sherpa = owlapi::vocabulary::OM::resolve("Sherpa");
 IRI move_to = owlapi::vocabulary::OM::resolve("MoveTo");

 std::vector<OWLCardinalityRestriction::Ptr> r_sherpa = ask.getCardinalityRestrictions(sherpa);
 std::vector<OWLCardinalityRestriction::Ptr> r_move_to = ask.getCardinalityRestrictions(move_to);

 try {
     // Check whether 'move_to' provides a subset of restriction existing for 'sherpa'
     csp::ResourceMatch::Solution solution = cs::ResourceMatch::solve(r_move_to, r_sherpa, ontology);
     std::cout << solution->toString() << std::endl;
 } catch(const std::runtime_error& e)
 {
     std::cout << "No solution found" << std::endl;
 }

 // or in compact form
 if(csp::ResourceMatch::isSupporting(sherpa, move_to, ontology);
 {
    std::cout << "Sherpa fulfills requirements for MoveTo" << std::endl;
 }
 \endverbatim
 */
class ResourceMatch : public Gecode::Space
{
    /// Required set of models
    ModelBound::List mRequiredModelBound;
    /// Available set of models
    ModelBound::List mAvailableModelBound;

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

    ResourceMatch* solve();

protected:
    ResourceMatch(const ModelBound::List& required,
            const ModelBound::List& provided,
            owlapi::model::OWLOntology::Ptr ontology);

    /**
     * Search support
     * This copy constructor is required for the search engine
     * and it has to provide a deep copy
     */
    ResourceMatch(ResourceMatch& s);

    /**
     * This method is called by the search engine
     */
    virtual Gecode::Space* copy(void);

public:

    typedef shared_ptr<ResourceMatch> Ptr;
    class Solution
    {
    public:
        std::string toString(uint32_t indent = 0) const;

        /**
         * Get the bound for a given model
         * \throws std::invalid_argument if model is not part of the solution
         */
        ModelBound::List getAssignments(const owlapi::model::IRI& model) const;

        /**
         * Set the list of resources that together fulfill the requirement of
         * the given model
         */
        void setAssignments(const ModelBound& model, const ModelBound::List& assignments) { mAssignments[model] = assignments; }
        /**
         * get complete AssignmentsMap
         */
        std::map<ModelBound, ModelBound::List> getAssignmentsMap() const {return mAssignments;}

        /**
         * Add the assignment to existing assignements of resources to fulfill the requirement
         * \param model requirement model
         * \param assignment model bound of available resources that are assigned to fulfill the requirement
         */
        void addAssignment(const ModelBound& model, const ModelBound& assignment) { mAssignments[model].push_back(assignment); }


        /**
         * Substract from a list of available resources
         * \throw invalid argument if operation would result in a negative
         * assignment
         * \return remaining resources
         */
        ModelBound::List substractMinFrom(const ModelBound::List& availableResources) const;

    private:
        // Assignments for this model type
        std::map<ModelBound, ModelBound::List> mAssignments;
    };

    ResourceMatch::Solution getSolution() const;

    void print(std::ostream& os) const;

    /**
     * Check if provider resources fulfill the model requirements
     * \return A solution that list the dedicated models being used an their
     * min/max bounds
     */
    static ResourceMatch::Solution solve(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& modelRequirements,
            const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& providerResources,
            owlapi::model::OWLOntology::Ptr ontology);

    static ResourceMatch::Solution solve(const ModelBound::List& required,
            const ModelBound::List& available,
            owlapi::model::OWLOntology::Ptr ontology);

    static ResourceMatch::Solution solve(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& modelRequirements,
            const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& providerResources,
            const OrganizationModelAsk& ontologyAsk);

    static ResourceMatch::Solution solve(const ModelBound::List& required,
            const ModelBound::List& available,
            const OrganizationModelAsk& ontologyAsk);

    /**
     * Create a string representation of this object
     * \return Stringified ResourceMatch object
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
     * \param providerModel
     * \param serviceModel
     * \param resourceMatch
     * \return True if the providerModel supports the serviceModel
     */
    static bool isSupporting(const owlapi::model::IRI& providerModel, const owlapi::model::IRI& serviceModel,
            owlapi::model::OWLOntology::Ptr ontology,
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has());


    static bool isSupporting(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& providerRestrictions,
            const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& serviceRestrictions,
            owlapi::model::OWLOntology::Ptr ontology);

    /**
     * Compute for a given model pool and possible models, the available
     * set of supported models, i.e., fulfilling the restrictions
     * \return List of provided services
     */
    static owlapi::model::IRIList filterSupportedModels(const ModelPool& modelPool,
            const owlapi::model::IRIList& serviceModels,
            owlapi::model::OWLOntology::Ptr ontology,
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has());
    /**
     * Compute for a given set of model and possible models, the available
     * set of supported models, i.e., fulfilling the restrictions
     * \return List of provided services
     */
    static owlapi::model::IRIList filterSupportedModels(const owlapi::model::IRIList& combination,
            const owlapi::model::IRIList& serviceModels, owlapi::model::OWLOntology::Ptr ontology,
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has());

    /**
     * Check if the model bound list contains minimum requirements, i.e.
     * if any of the model bounds min in > 0
     */
    static bool hasMinRequirements(const ModelBound::List& list);
};

} // end namespace reasoning
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_CSP_RESOURCE_MATCH_HPP
