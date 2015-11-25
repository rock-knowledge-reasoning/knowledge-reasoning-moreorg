#ifndef ORGANIZATION_MODEL_FUNCTIONALITY_MAPPING_HPP
#define ORGANIZATION_MODEL_FUNCTIONALITY_MAPPING_HPP

#include <set>
#include<organization_model/ModelPool.hpp>

namespace organization_model {

typedef std::set<ModelCombination> ModelCombinationSet;

/// Maps a 'combined system' to the functionality it can 'theoretically'
/// provide when looking at its resources
typedef std::map<ModelPool, owlapi::model::IRIList> Pool2FunctionMap;
typedef std::map<owlapi::model::IRI, ModelPoolSet > Function2PoolMap;

class FunctionalityMapping
{
    ModelPool mModelPool;
    owlapi::model::IRIList mFunctionalities;
    ModelPool mFunctionalSaturationBound;

    Pool2FunctionMap mPool2Function;
    Function2PoolMap mFunction2Pool;

public:
    FunctionalityMapping();

    /**
     * Create a functionality mapping
     *
     * \param modelPool the available resources
     * \param functionalities List of model to compute the mapping for
     * \param functionalSaturationBound The functionalSaturationBound that
     * should be taken into account
     */
    FunctionalityMapping(const ModelPool& modelPool,
        const owlapi::model::IRIList& functionalities,
        const ModelPool& functionalSaturationBound);

    const owlapi::model::IRIList& getFunction(const ModelPool& modelPool) const;
    const ModelPoolSet& getModelPools(const owlapi::model::IRI& iri) const;

    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }
    const ModelPool& getModelPool() const { return mModelPool; }

    void setFunctionalSaturationBound(const ModelPool& bounds) { mFunctionalSaturationBound = bounds; }
    const ModelPool& getFunctionalSaturationBound() const { return mFunctionalSaturationBound; }

    void addFunction(const ModelPool& modelPool, const owlapi::model::IRIList& functionModels);

    void add(const ModelPool& modelPool, const owlapi::model::IRIList& functionModels);

    std::string toString() const;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FUNCTIONALITY_MAPPING_HPP
