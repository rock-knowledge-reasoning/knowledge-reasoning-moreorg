#ifndef ORGANIZATION_MODEL_ASK_HPP
#define ORGANIZATION_MODEL_ASK_HPP

#include <organization_model/OrganizationModel.hpp>

namespace organization_model {

class OrganizationModelAsk
{
public:
    OrganizationModelAsk(OrganizationModel::Ptr om, const ModelPool& modelPool = ModelPool());

    /**
     * Retrieve the list of all known service models
     */
    owlapi::model::IRIList getServiceModels() const;

    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }

    const Combination2FunctionMap& getCombination2FunctionMap() const { return mCombination2Function; }

    const Function2CombinationMap& getFunction2CombinationMap() const { return mFunction2Combination; }

    /**
     * Get the set of resources (as combination of models) that should support a given
     * list of services
     * That means, that services are either supported by separate systems or 
     * combined systems
     * \param services should be a set of services / service models
     * \return available resources to support this set of services
     */
    std::set<ModelCombinationSet> getResourceSupport(const ServiceSet& services) const;

    /**
     * Check if a service can be supported by a model -- given enough instances
     * of this model are provided 
     * \see getFunctionalSaturationPoint in order to find the minimum number
     * required to provide the functionality (if full support can be achieved)
     */
    bool canProvideFullSupport(const Service& service, const owlapi::model::IRI& model) const;

    /**
     * Check if a service can be at least partially supported by a model
     * \see getFunctionalSaturationPoint in order to find the maximum number
     * of model instances that actually contribute to enable the functionality 
     * (otherwise they only contribute to a higher redundancy)
     */
    bool canProvidePartialSupport(const Service& service, const owlapi::model::IRI& model) const;

    /**
     *  Depending on the contribution of a model to the service the functional
     *  saturation point can be interpreted differently:
     *  1. when all requires resources are provided by the model:
     *     -- check how many instances are required of this model to achieve
     *     full functionality
     *  2. when only a partial set of resource are provided:
     *     -- check how many instances are actually contributing to enable the
     *     functionality (and when switching into providing redundancy only)
     *  \return number of instances required for functional saturation
     */
    uint32_t getFunctionalSaturationPoint(const Service& service, const owlapi::model::IRI& model) const {return 0;}

    /**
     * Get the set of resources
     * \return available resources to support this set of services
     */
    std::set<ModelCombination> getMinimalResourceSupport(const ServiceSet& services) const;

    std::set<ModelCombination> getMinimalResourceSupport_v1(const ServiceSet& services) const;
    std::set<ModelCombination> getMinimalResourceSupport_v2(const ServiceSet& services) const;

    /**
     * Retrieve the minimum cardinality of given model to support the set of
     * services
     */
    uint32_t minRequiredCardinality(const ServiceSet& services, const owlapi::model::IRI& model) const;

    /**
     * Check if two ModelCombinations can be built from distinct resources
     */
    bool canBeDistinct(const ModelCombination& a, const ModelCombination& b) const;

    bool isSupporting(const ModelCombination& c, const ServiceSet& services) const;

protected:
    /**
     * Prepare the organization model for a given set of available models
     */
    void prepare();

    /**
     * Compute the functionality maps for the combination of models from a
     * limited set of available models
     */
    void computeFunctionalityMaps(const ModelPool& modelPool);

private:
    OrganizationModel::Ptr mpOrganizationModel;

    /// Maps a combination to its supported functionality
    Combination2FunctionMap mCombination2Function;
    /// Maps a functionality to combination that support this functionality
    Function2CombinationMap mFunction2Combination;

    /// Current set pool of models
    ModelPool mModelPool;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ASK_HPP
