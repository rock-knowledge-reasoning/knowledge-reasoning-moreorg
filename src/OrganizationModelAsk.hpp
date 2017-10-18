#ifndef ORGANIZATION_MODEL_ASK_HPP
#define ORGANIZATION_MODEL_ASK_HPP

#include <owlapi/model/OWLCardinalityRestriction.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include "SharedPtr.hpp"
#include "OrganizationModel.hpp"
#include "algebra/ResourceSupportVector.hpp"
#include "FunctionalityRequirement.hpp"

namespace organization_model {

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
     * Default constructor of the organization model ask
     * \param om OrganizationModel that is used as database
     * \param modelPool ModelPool that is used for bounding -- only agent models
     * will be considered
     * \param applyFunctionalSaturationBound Set bounding in order to
     * reduce computed combinations to the functional saturation bound --
     * otherwise all feasible combinations are computed
     */
    explicit OrganizationModelAsk(const OrganizationModel::Ptr& om,
            const ModelPool& modelPool = ModelPool(),
            bool applyFunctionalSaturationBound = false,
            double feasibilityCheckTimeoutInMs = 20000);

    /**
     * Retrieve the list of all known service models
     * \return list of all known service models
     */
    owlapi::model::IRIList getServiceModels() const;

    /**
     * Retrieve the list of all known functionalities
     * \return list of all known funtionalities
     */
    owlapi::model::IRIList getFunctionalities() const;

    /**
     * Retrieve the support of known functionalities
     */
    ModelPool getSupportedFunctionalities() const;

    /**
     * Set the model pool
     * A currently set model pool is required for some queries to the
     * organization model
     *
     */
    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }

    /**
     * Compute the functionality mapping for the model pool this object was
     * initialized with
     */
    FunctionalityMapping computeFunctionalityMapping(const ModelPool& pool, bool applyFunctionalSaturationBound = false) const;

    /*
     * Get the set of resource that support a given collection of
     * functionalities and accounting for the resource requirements
     * \return available combinations to support this set of functionalities
     * with the given constraints
     */
    ModelPool::Set getResourceSupport(const FunctionalityRequirement& functionalityRequirement) const;

    /**
     * Get the set of resource that support a given collection of
     * functionalities and accounting for the resource requirements
     * \return available combinations to support this set of functionalities
     * with the given constraints
     */
    ModelPool::Set getResourceSupport(const FunctionalityRequirement::Map& functionalityRequirements) const;

    /**
     * Get the set of resources (or combination thereof) that support a given
     * union of services
     * That means, that services are either supported by separate systems or
     * combined systems
     * \param functionalities should be a set of functionalities / functionality models
     * \return available resources (or combination thereof) to support this set of functionalities
     */
    ModelPool::Set getResourceSupport(const Functionality::Set& functionalities,
            const FunctionalityRequirement::Map& functionalityRequirements) const;

    /**
     * Get the set of resources (or combination thereof) that support a given
     * union of services
     * That means, that services are either supported by separate systems or
     * combined systems
     * \param functionalities should be a set of functionalities / functionality models
     * \return available resources (or combination thereof) to support this set of functionalities
     */
    ModelPool::Set getResourceSupport(const Functionality::Set& functionalities) const;

    /**
     * Get the set of resources that should support a given union of services,
     * bounded by the FunctionalSaturationBound, i.e., the minimum combination
     * of resources that support the given union of services
     * \return bound set of combinations
     */
    ModelPool::Set getBoundedResourceSupport(const Functionality::Set& functionalities) const;

    /**
     * Apply an upper bound of resources to an existing Set of model
     * combinations
     * \param upperBounds Bounding ModelPool
     * \return all combinations that operate within the bounds of the given
     * ModelPool
     * \todo move to ModelPool
     */
    ModelCombinationSet applyUpperBound(const ModelCombinationSet& combinations, const ModelPool& upperBounds) const;

    /**
     * Apply a lower bound of resources to an existing set of model
     * combinations, i.e., remove ModelCombinations that operate below the lower
     * bound
     * \param combinations
     * \param lowerBounds Bounding ModelPool
     * \return all combinations that operate within the bounds of the given
     * ModelPool
     */
    ModelCombinationSet applyLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBounds) const;

    /**
     * Apply a lower bound of resources to an existing set of model
     * combinations, i.e., remove ModelCombinations that operate below the lower
     * bound
     * \param combinations
     * \param lowerBounds Bounding ModelPool
     * \return all combinations that operate within the bounds of the given
     * ModelPool
     */
    ModelPool::Set applyLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBounds) const;

    /*
     * Enforces the minimum requirement by expanding missing models to
     * a model combination to fulfill the requirement,
     * Apply a lower bound of resources to an existing set of model
     * combinations
     * \param combinations
     * \param lowerBounds Bounding ModelPool
     * \return all combinations that operate now within the bounds of the given
     * ModelPool
     */
    ModelCombinationSet expandToLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBounds) const;

    /*
     * Enforces the minimum requirement by expanding missing models to
     * a model combination to fulfill the requirement,
     * Apply a lower bound of resources to an existing set of model
     * combinations
     * \param combinations
     * \param lowerBounds Bounding ModelPool
     * \return all combinations that operate now within the bounds of the given
     * ModelPool
     */
    ModelPool::Set expandToLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBounds) const;

    /**
     * Check how a list of functionalities is supported by a model if given cardinality
     * of this model is provided
     * \param functionalities Functionality to be available
     * \param model Model that is tested for support of this functionality
     * \param cardinalityOfModel Allow to specify a support for quantified
     * number of these services, i.e., factor of resources to be available
     * \see getFunctionalSaturationBound in order to find the minimum number
     * \return required to provide the functionality (if full support can be achieved)
     */
    algebra::SupportType getSupportType(const Functionality::Set& functionality,
            const owlapi::model::IRI& model,
            uint32_t cardinalityOfModel = 1) const;

    /**
     * Check how a service is supported by a model if given cardinality
     * of this model is provided
     * \param functionality Functionality to be available
     * \param model Model that is tested for support of this functionality
     * \param cardinalityOfModel Allow to specify a support for quantified
     * number of these services, i.e., factor of resources to be available
     * \see getFunctionalSaturationBound in order to find the minimum number
     * \return required to provide the functionality (if full support can be achieved)
     */
    algebra::SupportType getSupportType(const Functionality& functionality,
            const owlapi::model::IRI& model,
            uint32_t cardinalityOfModel = 1) const;

    /**
     * Check how a functionality is supported by a model pool, i.e. number of
     * models with cardinalities provided
     * \param functionality Functionality to be available
     * \param models ModelPool that is available or represent the combination of
     * systems
     * \return type of support
     */
    algebra::SupportType getSupportType(const Functionality::Set& functionalities,
            const ModelPool& models) const;

    /**
     * Check how a functionality is supported by a model pool, i.e. number of
     * models with cardinalities provided
     * \param functionality Functionality to be available
     * \param models ModelPool that is available or represent the combination of
     * systems
     * \return type of support
     */
    algebra::SupportType getSupportType(const Functionality& functionality,
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
     * \param functionality for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     */
    ModelPool getFunctionalSaturationBound(const Functionality& functionality) const;

    /**
     * Compute the upper bound for the cardinality of each resource model
     * to support the given set of services (union of services)
     * This function does take into account the model pool, but computes
     * a global bound
     * \param functionalities set of functionalities for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     * \throw std::invalid_argument when the model pool is empty
     */
    ModelPool getFunctionalSaturationBound(const Functionality::Set& functionalities) const;

    /**
     * Compute the upper bound for the cardinality of each resource model
     * to support the given set of services (union of services) under
     * consideration of the given constraints
     * \param functionalities set of functionalities for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     * \throw std::invalid_argument when the model pool is empty
     */
    ModelPool getFunctionalSaturationBound(const Functionality::Set& functionalities, const FunctionalityRequirement& constraints) const;

    /**
     * Compute the upper bound for the cardinality of each resource model
     * to support the given set of services (union of services) under
     * consideration of the given constraints
     * \param functionalities set of functionalities for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     * \throw std::invalid_argument when the model pool is empty
     */
    ModelPool getFunctionalSaturationBound(const Functionality::Set& functionalities, const FunctionalityRequirement::Map& constraints) const;
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
     * Check is the model combination supports a given set of services
     * \return True if the combination support the set of services, false
     * otherwise
     */
    bool isSupporting(const ModelPool& modelPool, const Functionality::Set& functionalities) const;

    /**
     * Check is the model combination supports a given set of services
     * \return True if the combination support the set of services, false
     * otherwise
     */
    bool isSupporting(const ModelPool& modelPool, const Functionality& functionality) const;

    /**
     * Check if the model supports a service
     * \return true if the robot supports the set of services
     */
    bool isSupporting(const owlapi::model::IRI& model, const Functionality& functionality) const;

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
     */
    bool isMinimal(const ModelPool& modelPool, const Functionality::Set& functionalities) const;

    /**
     * Get the data property value of the complete combination given by the pool
     * \throw std::runtime_error if the given data propery does not exist
     */
    double getDataPropertyValue(const ModelPool& modelPool, const owlapi::model::IRI& dataProperty) const;

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

    ModelPool::Set filterNonMinimal(const ModelPool::Set& modelPoolSet, const Functionality::Set& functionalities) const;

    double getScalingFactor(const ModelPool& modelPool, const FunctionalityRequirement& functionalityRequirement, bool doCheckSupport=false) const;

    std::vector<double> getScalingFactors(const ModelPool::Set& modelPoolSet, const FunctionalityRequirement& functionalityRequirement, bool doCheckSupport=false) const;
    /**
     *
     * TODO: proper implementation of min/max/eq constraints -- e.g. to detect
     * validity of constraints (use Gecode::IntVar propagation)
     */
    static void updateScalingFactor(std::vector<double>& factors, size_t idx, double newValue);

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
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ASK_HPP
