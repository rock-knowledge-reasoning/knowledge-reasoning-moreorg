#ifndef ORGANIZATION_MODEL_ASK_HPP
#define ORGANIZATION_MODEL_ASK_HPP

#include <owlapi/model/OWLCardinalityRestriction.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>

#include "SharedPtr.hpp"
#include "OrganizationModel.hpp"
#include "algebra/ResourceSupportVector.hpp"
#include "Algebra.hpp"
#include "vocabularies/OM.hpp"

namespace moreorg {

class Agent;
class ResourceInstance;

/**
 * \class OrganizationModelAsk
 * \brief This class allows to create query object to reason about and retrieve information
 * from an organization model
 *
 * \details
 * The ModelPool describes a set of countable (reusable) resources
 */
class OrganizationModelAsk
{
    friend class algebra::ResourceSupportVector;

public:
    typedef shared_ptr<OrganizationModelAsk> Ptr;

    /**
     * Default constructor for an instance of organization model ask
     * Without setting any organization model etc. this object
     * is only useful for lazy initialization
     */
    OrganizationModelAsk();


    /**
     * Default constructor of the organization model ask
     * \param om OrganizationModel that is used as database
     * \param modelPool ModelPool that is used for bounding -- only agent models
     * will be considered
     * \param applyFunctionalSaturationBound Set bounding in order to
     * \param feasibilityCheckTimeoutInMs Allow to limit the time for
     * reduce computed combinations to the functional saturation bound --
     * otherwise all feasible combinations are computed
     */
    explicit OrganizationModelAsk(const OrganizationModel::Ptr& om,
            const ModelPool& modelPool = ModelPool(),
            bool applyFunctionalSaturationBound = false,
            double feasibilityCheckTimeoutInMs = 20000,
            const owlapi::model::IRI& interfaceBaseClass =
            vocabulary::OM::resolve("ElectroMechanicalInterface"),
            size_t neighbourHood = 3);

    static const OrganizationModelAsk& getInstance(const OrganizationModel::Ptr& om,
            const ModelPool& modelPool = ModelPool(),
            bool applyFunctionalSaturationBound = false,
            double feasibilityCheckTimeoutInMs = 20000,
            const owlapi::model::IRI& interfaceBaseClass =
            vocabulary::OM::resolve("ElectroMechanicalInterface"),
            size_t neighbourHood = 3
            );

    /**
     * Retrieve the list of all known agent models
     * \return list of all known agent models
     */
    owlapi::model::IRIList getAgentModels() const;

    /**
     * Retrieve the list of all known agent properties
     * \return list of all known agent properties
     */
    owlapi::model::IRIList getAgentProperties(const owlapi::model::IRI& model = vocabulary::OM::Agent()) const;

    /**
     * Retrieve the list of all known service models
     * \return list of all known service models
     */
    owlapi::model::IRIList getServiceModels() const;

    /**
     * Retrieve the list of all known functionalities
     * \return list of all known generally known functionalities
     */
    owlapi::model::IRIList getFunctionalities() const;

    /**
      * Retrieve the list of functionalities based on the existing organization
      * \throw std::invalid_argument if pool is not a subset of the model pool
      * which has been use for initialization of the OrganizationModelAsk object
      * \return list of all functionalities available for a given model pool
      */
    owlapi::model::IRIList getSupportedFunctionalities(const ModelPool& pool) const;

    /**
     * Retrieve the support of known functionalities, i.e., which
     * functionalities can be supported with the used model pool in general,
     * without further specifying under what constraints
     * \return model pool, where all cardinalities for available functionalites will be set to 1
     */
    ModelPool getSupportedFunctionalities() const;

    /**
     * Set the model pool for the organization model
     * A currently set model pool is required for most queries to the
     * organization model
     */
    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }

    /**
     * Get the model pool associated with this ask object
     * \return model pool
     */
    const ModelPool& getModelPool() const { return mModelPool; }

    /**
     * Compute the functionality mapping for currently set model pool
     */
    FunctionalityMapping computeFunctionalityMapping(const ModelPool& pool, bool applyFunctionalSaturationBound = false) const;

    /*
     * Get the set of resources that support a given collection of
     * functionalities while accounting for the resource requirements
     * \param resource A resource including constraints
     * \return available combinations to support this set of functionalities
     * with the given constraints
     */
    ModelPool::Set getResourceSupport(const Resource& resource) const;

    /**
     * Get the set of resources that support a given collection of
     * functionalities while accounting for the resource requirements
     * \param resources A set of resource (here: functionalities and their
     * constraints)
     * \return available combinations to support this set of functionalities
     * with the given constraints
     */
    ModelPool::Set getResourceSupport(const Resource::Set& resources) const;

    /**
     * Get the set of resources that should support a given union of services,
     * bounded by the FunctionalSaturationBound, i.e., the minimum combination
     * of resources that support the given union of services
     * \return bound set of feasible combinations
     */
    ModelPool::Set getBoundedResourceSupport(const Resource::Set& resources) const;

    /**
     * Check how a list of functionalities is supported by a model if given cardinality
     * of this model is provided
     * \param resources Resource to be available
     * \param model Model that is tested for support of this functionality
     * \param cardinalityOfModel Allow to specify a support for quantified
     * number of instances of these resource models, i.e., factor of resources to be available
     * \see getFunctionalSaturationBound in order to find the minimum number
     * \return required to provide the functionality (if full support can be achieved)
     */
    algebra::SupportType getSupportType(const Resource::Set& resources,
            const owlapi::model::IRI& model,
            uint32_t cardinalityOfModel = 1) const;

    /**
     * Check how a service is supported by a model if given cardinality
     * of this model is provided
     * \param resource Resource to be available
     * \param model Model that is tested for support of this functionality
     * \param cardinalityOfModel Allow to specify a support for quantified
     * number of resources, i.e., acts as factor for contained resources to be available
     * \see getFunctionalSaturationBound in order to find the minimum number
     * \return required to provide the functionality (if full support can be achieved)
     */
    algebra::SupportType getSupportType(const Resource& resource,
            const owlapi::model::IRI& model,
            uint32_t cardinalityOfModel = 1) const;

    /**
     * Check how a resource (functionality) is supported by a model pool, i.e. number of
     * models with cardinalities provided
     * \param functionalities Set of functionalities to be available
     * \param models ModelPool that is available and represents the combination of
     * systems
     * \return type of support
     */
    algebra::SupportType getSupportType(const Resource::Set& resources,
            const ModelPool& models) const;

    /**
     * Check how a functionality is supported by a model pool, i.e. number of
     * models with cardinalities provided
     * \param resource Resource to be available
     * \param models ModelPool that is available and represents the combination of
     * systems
     * \return type of support
     */
    algebra::SupportType getSupportType(const Resource& resource,
            const ModelPool& models) const;

    /**
     *  Depending on the contribution of a model to the service the functional
     *  saturation point can be interpreted differently:
     *  1. when all required resources (FULL_SUPPORT) are provided by the model:
     *     -- check how many instances are required of this model to achieve
     *     full functionality
     *  2. when only a partial set of resources is provided (PARTIAL_SUPPORT)
     *     -- check how many instances are actually contributing to enable the
     *     functionality (and when switching into providing redundancy only)
     *  \param requirementModel model that defines the requirement to check for
     *  \param model Provider model for which the saturation bound is computed
     *  \return number of instances required for functional saturation
     */
    uint32_t getFunctionalSaturationBound(const owlapi::model::IRI& requirementModel, const owlapi::model::IRI& model) const;

    /**
     * Compute the upper bound for the cardinality of each resource model
     * for a given service
     * This function does take into account the model pool, but computes
     * a global bound
     * \param resource Resource/functionality for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     */
    ModelPool getFunctionalSaturationBound(const Resource& resource) const;

    /**
     * Compute the upper bound for the cardinality of each resource model
     * to support the given set of services (union of services)
     * This function does take into account the model pool, but computes
     * a global bound
     * \param resources Set of resources (formerly: functionalities) for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     * \throw std::invalid_argument when the model pool is empty
     */
    ModelPool getFunctionalSaturationBound(const Resource::Set& resources) const;

    ///**
    // * Get the set of resources
    // * \return available resources to support this set of services
    // */
    //std::set<ModelCombination> getMinimalResourceSupport(const ServiceSet& services) const;

    ///**
    // * This function computes the minimal resource support for a set of services
    // * by analyzing the existing ResourceSupport for each service and
    // */
    //std::set<ModelCombination> getMinimalResourceSupport_v1(const ServiceSet& services) const;
    //std::set<ModelCombination> getMinimalResourceSupport_v2(const ServiceSet& services) const;

    ///**
    // * Retrieve the minimum cardinality of given model to support the set of
    // * services
    // */
    //uint32_t minRequiredCardinality(const ServiceSet& services, const owlapi::model::IRI& model) const;

    /**
     * Check if two ModelCombinations can be built from distinct resources,
     * i.e. check if the current model pool provides enough resources for both
     * combinations
     * \return True if both model combinations can be maintained in parallel by
     * the existing model pool
     */
    bool canBeDistinct(const ModelCombination& a, const ModelCombination& b) const;

    /**
     * Get the intersection of model pools for a given set of resources
     *
     */
    ModelPool::Set getIntersection(const Resource::Set& resources) const;

    /**
     * Check is the given model pool supports a given set of resources
     * \param modelPool Model pool to query
     * \param resources Resources for which support is questioned
     * \return True if the model pool supports the set of services, false
     * otherwise
     */
    bool isSupporting(const ModelPool& modelPool,
            const Resource::Set& resources,
            double feasibilityCheckTimeoutInMs = 20
            ) const;

    /**
     * Check is the model combination supports a resource
     * \return True if the combination support the set of services, false
     * otherwise
     */
    bool isSupporting(const ModelPool& modelPool, const Resource& resource) const;

    /**
     * Check if the model supports a resource (service,functionality)
     * \return true if the robot supports the set of services
     */
    bool isSupporting(const owlapi::model::IRI& model, const Resource& resource) const;

    /**
     * Provide debug information about the status of this object
     * \return String representation of this query object
     */
    std::string toString() const;

    /**
     * Prepare the organization model for a given set of available models
     * \param modelPool Provide a ModelPool and use the second parameter to
     * decided whether to use it for inferring the functional saturation bound
     * \param applyFunctionalSaturationBound Set to true if all queries to this
     * object should take into account the functional saturation bound
     */
    void prepare(const ModelPool& modelPool, bool applyFunctionalSaturationBound = false);

    /**
     * Return ontology that relates to this Ask object
     * \return underlying OWLOntologyAsk object
     */
    owlapi::model::OWLOntologyAsk ontology() const { return mOntologyAsk; }

    /**
     * Return organization model that relates to this ask object
     * \return underlying OWLOrganizationModelAsk object
     */
    OrganizationModel::Ptr getOrganizationModel() const { return mpOrganizationModel; }

    /**
     * Get the functionality mapping of the current OrganizationModelAsk object
     * \return FunctionalityMapping
     */
    const FunctionalityMapping& getFunctionalityMapping() { return mFunctionalityMapping; }

    /**
     * Check if a model pool exceeds the saturation bound with respect to a
     * given functionality set
     * \return false, if the set of resources is not fully supported or the set
     * contains redundancies, i.e. return a single resource model provides
     * full support, but model pool contains more (thus redundant models),
     * false when partial support model overall exceed the minimum required set
     *
     */
    bool isMinimal(const ModelPool& modelPool, const Resource::Set& resources) const;

    /**
     * Add a functionality mapping and validate the structural consistency
     * \param minimalOnly If true a mapping is only added, if the combination is minimal
     * \return true when add successfully, false otherwise (meaning the
     * combination was not structurally consistent)
     */
    bool addFunctionalityMapping(FunctionalityMapping& functionalityMapping,
            const ModelPool& modelPool,
            const owlapi::model::IRI& functionality,
            bool minimalOnly = true) const;

    /**
     * Get the data property value of the complete combination given by the pool
     * \param dataProperty
     * \throw std::runtime_error if the given data propery does not exist
     */
    double getDataPropertyValue(const ModelPool& modelPool,
            const owlapi::model::IRI& dataProperty,
            Algebra::OperationType type = Algebra::SUM_OP) const;

    /**
     * Get the property value for a given target class, which is either given
     * through a cardinality restriction or a data property assignment
     */
    double getPropertyValue(const ModelPool& modelPool,
            const owlapi::model::IRI& property) const;

    std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> getRequiredCardinalities(
            const ModelPool& modelPool,
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has()) const;

    std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> getAvailableCardinalities(
            const ModelPool& modelPool,
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has()) const;

    /**
     * Get all (joined) restriction that hold for a given model pool
     * \param modelPool
     * \param type
     * \param min2Max When retrieving the cardinalities, e.g., in a mixed setup
     * for functionalities and model pools, then functionalities define
     * requirement in terms of Min cardinalities, and agents use Max cardinalities.
     * To allow proper computation of the metrics, adapt min to max cardinalities in that context
     */
    std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> getCardinalityRestrictions(const ModelPool& modelPool,
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has(),
            owlapi::model::OWLCardinalityRestriction::OperationType type = owlapi::model::OWLCardinalityRestriction::SUM_OP,
            bool max2Min = false) const;

    /**
     * Compute the related instances for a given agent
     */
    std::vector<ResourceInstance> getRelated(const Agent& agent,
            const owlapi::model::IRI& qualification = vocabulary::OM::Resource(),
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has()) const;

    /**
     * Compute the instance list for a given model
     */
    std::vector< shared_ptr<ResourceInstance> > getRelated(const owlapi::model::IRI& model,
            const owlapi::model::IRI& qualification = vocabulary::OM::Resource(),
            const owlapi::model::IRI& objectProperty = vocabulary::OM::has()) const;

    /**
     * Allow only subclasses of a particular type in a model pool
     */
    ModelPool allowSubclasses(const ModelPool& modelPool,
            const owlapi::model::IRI& parent) const;

    /**
     * Check feasibility of a given model pool
     */
    bool isFeasible(const ModelPool& modelPool,
            double feasibilityCheckTimeoutInMs = 0.0) const;

    /**
     * Find a feasible coalition structure where all systems support a list of
     * functionality
     */
    ModelPool::List findFeasibleCoalitionStructure(const ModelPool& modelPool,
            const Resource::Set& supportedResourceSet,
            double feasibilityCheckTimeoutInMs);

    /**
      * Get all property values describing a component a an agent model
      * \return object properties map by the property path, where the key is
      * [instance,dataProperty]
      */
    std::map< owlapi::model::IRI, std::map<owlapi::model::IRI, double> > getPropertyValues(const
            owlapi::model::IRI& agent,
            const owlapi::model::IRI& componentKlass,
            const owlapi::model::IRI& relation =
            vocabulary::OM::has()) const;

protected:

    /**
     * Get the ResourceSupportVector for a given list of functionality models
     * \param functions Names of the function model
     * \param filterLabels Model names corresponding to the dimensions of the
     * support vector (resulting vector will be limited to these filterLabels (subclasses of this model will be included in the cardinality))
     * \param useMaxCardinality If true, the support vector consists of all max
     * cardinality value, if false, min values are uses
     * \see algebra::ResourceSupportVector
     * \return  the support vector
     */
    algebra::ResourceSupportVector getSupportVector(const owlapi::model::IRIList& functions,
        const owlapi::model::IRIList& filterLabels = owlapi::model::IRIList(),
        bool useMaxCardinality = false) const;

    /**
     * Get the ResourceSupportVector for a given model
     * \param model Name of the model
     * \param filterLabels Model names corresponding to the dimensions of the
     * support vector (resulting vector will be limited to these filterLabels (subclasses of this model will be included in the cardinality))
     * \param useMaxCardinality If true, the support vector consists of all max
     * cardinality value, if false, min values are uses
     * \see algebra::ResourceSupportVector
     * \return  the support vector
     */
    algebra::ResourceSupportVector getSupportVector(const owlapi::model::IRI& model,
        const owlapi::model::IRIList& filterLabels = owlapi::model::IRIList(),
        bool useMaxCardinality = false) const;

    /**
     * Get the support vector provided some MinMax bounds
     *
     * \param filterLabels Model names corresponding to the dimensions of the
     * support vector
     * \param useMaxCardinality If true, the support vector consists of all max
     * cardinality value, if false, min values are uses
     */
    algebra::ResourceSupportVector getSupportVector(const std::map<owlapi::model::IRI,
        owlapi::model::OWLCardinalityRestriction::MinMax>& modelBounds,
        const owlapi::model::IRIList& filterLabels = owlapi::model::IRIList(),
        bool useMaxCardinality = false ) const;

    owlapi::model::IRIList filterSupportedModels(const owlapi::model::IRIList& combinations,
        const owlapi::model::IRIList& serviceModels);

    FunctionalityMapping computeBoundedFunctionalityMapping(const ModelPool& pool, const owlapi::model::IRIList& functionalityModels) const;
    FunctionalityMapping computeUnboundedFunctionalityMapping(const ModelPool& pool, const owlapi::model::IRIList& functionalityModels) const;

    ModelPool::Set filterNonMinimal(const ModelPool::Set& modelPoolSet, const Resource::Set& resources) const;

    /**
     * Given a modelPool (which supports/satisfies the requested functionality) provide an
     * indication on how many of these composite agents (represented by this
     * modelPool) are needed to fulfill the requirement
     * Relies on proper CSP-based bound checking to merge the provided
     * functionality requirements and embedded property constraints
     * \param modelPool ModelPool describing a composite system
     * \param resource Functionality description including constraints, which
     * shall be provided by this modelPool
     * \param doCheckSupport Whether the general support for a functionality
     * should be checked first
     * \return the minimum scaling factor to achieve the desired support for a
     * resource
     */
    double getScalingFactor(const ModelPool& modelPool,
            const Resource& resource,
            bool doCheckSupport=false
            ) const;

    /**
     * Given a number of modelPools (which supports the requested functionality) provide an
     * indication on how many of these composite agents (represented by this
     * modelPool) are needed to fulfill the requirement
     * \return scaling factor
     */
    std::vector<double> getScalingFactors(const ModelPool::Set& modelPoolSet,
            const Resource& resource,
            bool doCheckSupport=false
            ) const;

    /**
     * Update the list of scaling factors, which tell how many instances of a
     * 'model pool' are required to support a requested (set of) functionality
     * \see getScalingFactor
     */
    static void updateScalingFactor(std::vector<double>& factors,
            size_t idx,
            double newValue);

    /**
     * Check if there is functionality support in the neighborhood of a
     * base model pool.
     * The neighborhood is defined by a permitted upper bound on additional model
     * instances and a maximum number of instances
     */
    void exploreNeighbourhood(FunctionalityMapping& functionalityMapping,
            const ModelPool& basePool,
            const ModelPool& maxDelta,
            const owlapi::model::IRI& functionality,
            size_t maxAddedInstances) const;

    /**
      * Get the interface base class that is used for performing
      * feasibility checking
      */
    const owlapi::model::IRI& getInterfaceBaseClass() const { return mInterfaceBaseClass; }

private:
    OrganizationModel::Ptr mpOrganizationModel;
    owlapi::model::OWLOntologyAsk mOntologyAsk;
    bool mApplyFunctionalSaturationBound;

    /// Maps a combination to its supported functionality and vice versa
    FunctionalityMapping mFunctionalityMapping;

    /// Current pool of models, i.e.
    /// how many instances of type X,Y
    /// to be considered
    ModelPool mModelPool;

    double mFeasibilityCheckTimeoutInMs;
    /// Size of the neighbourhood for the exploration of structurally feasible
    /// compositions, starting from the functional saturation bound
    size_t mStructuralNeighbourhood;
    owlapi::model::IRI mInterfaceBaseClass;
    static std::vector<OrganizationModelAsk> msOrganizationModelAsk;

    mutable std::map<owlapi::model::IRI, std::vector< shared_ptr<ResourceInstance> > > mRelatedResourceCache;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_ASK_HPP
