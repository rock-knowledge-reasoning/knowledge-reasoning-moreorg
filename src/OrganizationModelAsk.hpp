#ifndef ORGANIZATION_MODEL_ASK_HPP
#define ORGANIZATION_MODEL_ASK_HPP

#include <organization_model/OrganizationModel.hpp>

namespace organization_model {

typedef owlapi::model::IRIList ModelCombination;
typedef std::vector<ModelCombination> ModelCombinationList;
typedef std::map<owlapi::model::IRI, size_t> ModelPool;
typedef std::map<owlapi::model::IRI, int> ModelPoolDelta;

/// Maps a 'combined system' to the functionality it can 'theoretically'
/// provide when looking at its resources
typedef std::map<ModelCombination, owlapi::model::IRIList> Combination2FunctionMap;
typedef std::map<owlapi::model::IRI, ModelCombinationList > Function2CombinationMap;

class OrganizationModelAsk
{
public:
    OrganizationModelAsk(OrganizationModel::Ptr om, const ModelPool& modelPool = ModelPool());

    /**
     * Retrieve the list of all known service models
     */
    owlapi::model::IRIList getServiceModels() const;

    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }

    static std::string toString(const Combination2FunctionMap& combinationFunctionMap);

    static std::string toString(const Function2CombinationMap& functionCombinationMap);

    const Combination2FunctionMap& getCombination2FunctionMap() const { return mCombination2Function; }

    const Function2CombinationMap& getFunction2CombinationMap() const { return mFunction2Combination; }

    /**
     * Get the minimal set of resources (as combination of models) that should support a given
     * list of services
     * That means, that services are either supported by separate systems or 
     * combined systems
     * \param services should be a set of services / service models
     * \return available resources to support this set of services
     */
    std::vector<ModelCombinationList> getMinimalResourceSupport(const ServiceList& services);

    /**
     * Check if two ModelCombinations can be built from distinct resources
     */
    bool canBeDistinct(const ModelCombination& a, const ModelCombination& b) { throw std::runtime_error("Not impl"); }

private:
    /**
     * Prepare the organization model for a given set of available models
     */
    void prepare();

    OrganizationModel::Ptr mpOrganizationModel;

    /// Maps a combination to its supported functionality
    Combination2FunctionMap mCombination2Function;
    /// Maps a functionality to combination that support this functionality
    Function2CombinationMap mFunction2Combination;

    /// Current set pool of models
    ModelPool mModelPool;

    /**
     * Compute the functionality maps for the combination of models from a
     * limited set of available models
     */
    void computeFunctionalityMaps(const ModelPool& modelPool);

    static ModelPool combination2ModelPool(const ModelCombination& combination);
    static ModelCombination modelPool2Combination(const ModelPool& pool);
    static ModelPoolDelta delta(const ModelPool& a, const ModelPool& b);

};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ASK_HPP
