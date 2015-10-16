#ifndef ORGANIZATION_MODEL_ASK_HPP
#define ORGANIZATION_MODEL_ASK_HPP

#include <owlapi/model/OWLCardinalityRestriction.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <organization_model/algebra/ResourceSupportVector.hpp>

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
    typedef boost::shared_ptr<OrganizationModelAsk> Ptr;

    OrganizationModelAsk(OrganizationModel::Ptr om,
            const ModelPool& modelPool = ModelPool(),
            bool applyFunctionalSaturationBound = false);

    /**
     * Retrieve the list of all known service models
     * \return list of all known service models
     */
    owlapi::model::IRIList getServiceModels() const;

    /**
     * Set the model pool
     * A currently set model pool is required for some queries to the
     * organization model
     *
     */
    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }

    /**
     * Get the functionality mapping for the model pool this object was
     * initialized with
     */
    FunctionalityMapping getFunctionalityMapping(const ModelPool& pool, bool applyFunctionalSaturationBound = false) const;

    /**
     * Get the set of resources (or combination thereof) that support a given
     * union of services
     * That means, that services are either supported by separate systems or
     * combined systems
     * \param services should be a set of services / service models
     * \return available resources (or combination thereof) to support this set of services
     */
    ModelCombinationSet getResourceSupport(const ServiceSet& services) const;

    /**
     * Get the set of resources that should support a given union of services,
     * bounded by the FunctionalSaturationBound, i.e., the minimum combination
     * of resources that support the given union of services
     * \return bound set of combinations
     */
    ModelCombinationSet getBoundedResourceSupport(const ServiceSet& services) const;

    /**
     * Apply an upper bound of resources to an existing Set of model
     * combinations
     * \param upperBounds Bounding ModelPool
     * \return all combinations that operate within the bounds of the given
     * ModelPool
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


    /**
     * Check how a service is supported by a model if given cardinality
     * of this model is provided
     * \see getFunctionalSaturationBound in order to find the minimum number
     * \return required to provide the functionality (if full support can be achieved)
     */
    algebra::SupportType getSupportType(const Service& service,
            const owlapi::model::IRI& model,
            uint32_t cardinalityOfModel = 1) const;

    /**
     *  Depending on the contribution of a model to the service the functional
     *  saturation point can be interpreted differently:
     *  1. when all required resources (FULL_SUPPORT) are provided by the model:
     *     -- check how many instances are required of this model to achieve
     *     full functionality
     *  2. when only a partial set of resources is provided (PARTIAL_SUPPORT)
     *     -- check how many instances are actually contributing to enable the
     *     functionality (and when switching into providing redundancy only)
     *  \param service Service to check the saturation bound for
     *  \param model Model Model IRI for which the saturation bound is computed
     *  \return number of instances required for functional saturation
     */
    uint32_t getFunctionalSaturationBound(const Service& service, const owlapi::model::IRI& model) const;

    /**
     * Compute the upper bound for the cardinality of each resource model
     * for a given service
     * This function does take into account the model pool, but computes
     * a global bound
     * \param service for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     */
    ModelPool getFunctionalSaturationBound(const Service& service) const;

    /**
     * Compute the upper bound for the cardinality of each resource model
     * to support the given set of services (union of services)
     * This function does take into account the model pool, but computes
     * a global bound
     * \param set of services for which the functional saturation bound needs to
     * be computed
     * \return Cardinality bound for resource models
     * \throw std::invalid_argument when the model pool is empty
     */
    ModelPool getFunctionalSaturationBound(const ServiceSet& services) const;

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
     * Check if two ModelCombinations can be built from distinct resources
     */
    bool canBeDistinct(const ModelCombination& a, const ModelCombination& b) const;

    bool isSupporting(const ModelCombination& c, const ServiceSet& services) const;

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
protected:

    /**
     * Get the functionality maps for the combination of models from a
     * (limited) set of available models
     */
     FunctionalityMapping getFunctionalityMapping(const ModelPool& modelPool) const;

    /**
     * Get the support vector for a given model
     * Does not (!) account for (sub)class relationship
     * \param model Name of the model
     * \param filterLabels Model names corresponding to the dimensions of the
     * support vector
     * \param useMaxCardinality If true, the support vector consists of all max
     * cardinality value, if false, min values are uses
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
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ASK_HPP
