#ifndef OWL_OM_ORGANIZATION_MODEL_HPP
#define OWL_OM_ORGANIZATION_MODEL_HPP

#include <stdint.h>
#include <owl_om/Ontology.hpp>
#include <base/Time.hpp>

namespace owl_om {

typedef std::vector<IRIList> CandidatesList;

struct InterfaceConnection
{
    InterfaceConnection() {}

    InterfaceConnection(const IRI& interface0, const IRI& interface1)
        : begin(interface0)
        , end(interface1)
    {}

    IRI begin;
    IRI end;

    IRIList parents;

    void addParent(const IRI& parent);
    bool sameParents(const InterfaceConnection& other) const;

    /**
     * Test if the two interface connections use the same interface
     */
    bool useSameInterface(const InterfaceConnection& other) const;

    bool operator<(const InterfaceConnection& other) const;

    std::string toString() const;
};

typedef std::vector< InterfaceConnection > InterfaceConnectionList;
typedef std::vector< InterfaceConnectionList > InterfaceCombinationList;

/**
 * Statistic of the organization model engine
 */
struct Statistics
{
    uint32_t upperCombinationBound;
    uint32_t numberOfInferenceEpochs;
    base::Time timeElapsed;

    IRIList interfaces;
    InterfaceConnectionList links;
    InterfaceCombinationList linkCombinations;

    uint32_t constraintsChecked;

    IRIList actorsKnown;
    IRIList actorsInferred;

    IRIList actorsCompositePrevious;
    IRIList actorsCompositePost;

    IRIList actorsCompositeModelPrevious;
    IRIList actorsCompositeModelPost;

    std::string toString() const;
};

std::ostream& operator<<(std::ostream& os, const InterfaceConnection& connection);
std::ostream& operator<<(std::ostream& os, const InterfaceConnectionList& list);
std::ostream& operator<<(std::ostream& os, const InterfaceCombinationList& list);

typedef std::map<IRI,IRI> RequirementsGrounding;

class Grounding
{
    RequirementsGrounding mRequirementToResourceMap;
public:
    Grounding(const RequirementsGrounding& grounding)
        : mRequirementToResourceMap(grounding)
    {}

    const RequirementsGrounding& getRequirementsGrounding() { return mRequirementToResourceMap; }

    bool isComplete() const;

    static IRI ungrounded() { static IRI iri("?"); return iri; }

    std::string toString() const;
};


class OrganizationModel
{
public:

    typedef boost::shared_ptr<OrganizationModel> Ptr;
    typedef std::map< IRI, IRIList> IRI2IRIListCache;
    typedef std::map< IRI, IRI> IRI2IRICache;
    typedef std::map< std::pair<IRI,IRI>, IRIList> RelationCache;
    typedef std::map< std::pair<IRI,IRI>, bool> RelationPredicateCache;

    /**
     * Constructor to create an OrganizationModel from an existing description file
     * \param filename File name to an rdf/xml formatted ontology description file
     */
    OrganizationModel(const std::string& filename = "");

    /**
     * Update the organization model
     */
    void refresh();

    Ontology::Ptr ontology() { return mpOntology; }

    const Ontology::Ptr ontology() const { return mpOntology; }

    /**
     * Creation of an instance of a given elementary klass, which is modeled by a given model klass
     */
    void createInstance(const IRI& instanceName, const IRI& klass, const IRI& model);

    /**
     * Try to map requirements to provider
     * \param resourceRequirement Resource requirement
     * \param availableResources List of available resources
     * \param resourceProvider Optional label for the resource provider
     * \param requirementModel Optional label for requirement model that the requirements originate from
     * \return corresponding grounding, which need to be check on completness
     */
    Grounding resolveRequirements(const IRIList& resourceRequirements, const IRIList& availableResources, const IRI& resourceProvider = IRI(), const IRI& requirementModel = IRI()) const;

    // PREDICATES
    /**
     * Test if a set of interfaces is compatible (in general)
     * Test the relation 'compatibleWith'
     *
     */
    bool checkIfCompatible(const IRI& instance, const IRI& otherInstance);

    /**
     * Run inference to identify service that are 'provided'
     * by a set of actor
     * \param actors
     */
    void runInferenceEngine(const IRIList& actors);

    /**
     * Reduce list of actor to unique individuals, i.e. removing
     * alias and same individuals
     */
    IRIList compactActorList();

    /**
     * Get resource model of this instance
     * \return IRI of resource model
     */
    IRI getResourceModel(const IRI& instance) const;

    /**
     * Get the instance type that will/should be created when 
     * using a specific model
     *
     * This instance type is set using the 'models' property on the instance after
     * punning the model class
     * \param model Model name
     * \see http://www.w3.org/TR/owl2-new-features/#Simple_metamodeling_capabilities
     * \return IRI instance type for newly created instance of using model
     */
    IRI getResourceModelInstanceType(const IRI& model) const;

    /**
     * Check if both instance use the same resource model
     * \return True upon success, false otherwise
     */
    bool isSameResourceModel(const IRI& instance, const IRI& otherInstance) const;

    /**
     * Create new instance (ABox from existing model (model is also in ABox))
     * Model will be associated via modelledBy relation and subclassed from
     * classType, the name for this instance will be autogenerated
     * \param model New instance will be modelledBy this model
     * \param createDependantResources create dependants objects if required
     * \return IRI to new model instance, the name for this instance will automatically
     * generate based on the number of existing instances
     */
    IRI createNewFromModel(const IRI& model, bool createDependantResources = false) const;

    /**
     * Generate a combination list based on actor interface
     * \return List of interface combinations
     */
    InterfaceCombinationList generateInterfaceCombinations();

    /**
     * Compute upper bound for actor combinations
     * \return upper bound
     */
    uint32_t upperCombinationBound();

    /**
     * Statistics
     */
    Statistics getStatistics() { return mStats; }

    void setMaximumNumberOfLinks(uint32_t n) { mMaximumNumberOfLinks = n; }
    uint32_t getMaximumNumberOfLinks() { return mMaximumNumberOfLinks; }


private:

    void addProvider(const IRI& actor, const IRI& model);

    /**
     * Check if model is provider for a given model, i.e.
     * remaps requirements of actorModel as availableResources
     * and check whether this is sufficient for the given model
     * \return True upon success, false otherwise
     */
    bool isModelProvider(const IRI& actorModel, const IRI& model) const;

    /**
     * Cached version
     *
     * Checks if the associated actor model is providing the given model
     */
    bool isProviding(const IRI& actor, const IRI& model) const;

    /**
     * Cached version
     */
    IRIList allRelatedInstances(const IRI& actor, const IRI& relation) const;

    IRIList getModelRequirements(const IRI& model) const;

    /**
     * Creates a new clas of actor from the given set of actor
     * \return IRI of new actor, or empty IRI if the actor already exists
     */
    IRI createNewActor(const IRISet& actorSet, const InterfaceConnectionList& interfaceConnections, uint32_t id);

    /**
     * Infer new instance of a given class type, base on checking whether a given model
     * is fulfilled
     *
     * Will associate the given actor with each model
     * \param actor Actor to perform inference for, actor will in effect have two types of relationships:
     *     (a) provides model
     *     (b) has instance of the class type  (where instance is modelledBy the given model)
     * \param models List of models that should be check
     */
    IRIList infer(const IRI& actor, const IRIList& models);


    Ontology::Ptr mpOntology;

    Statistics mStats;

    mutable IRI2IRIListCache mModelRequirementsCache;
    mutable RelationCache mRelationsCache;
    mutable IRI2IRICache mResourceModelCache;
    mutable RelationPredicateCache mProviderCache;
    mutable RelationPredicateCache mCompatibilityCache;

    uint32_t mMaximumNumberOfLinks;
};

} // end namespace owl_om
#endif // OWL_OM_ORGANIZATION_MODEL_HPP
