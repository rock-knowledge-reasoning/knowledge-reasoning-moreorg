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
     * Get the set of resources
     * \return available resources to support this set of services
     */
    std::set<ModelCombination> getMinimalResourceSupport(const ServiceSet& services) const;

    /**
     * Check if two ModelCombinations can be built from distinct resources
     */
    bool canBeDistinct(const ModelCombination& a, const ModelCombination& b);

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
