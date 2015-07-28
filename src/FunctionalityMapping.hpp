#ifndef ORGANIZATION_MODEL_FUNCTIONALITY_MAPPING_HPP
#define ORGANIZATION_MODEL_FUNCTIONALITY_MAPPING_HPP

#include <set>
#include<organization_model/ModelPool.hpp>

namespace organization_model {

typedef std::vector<ModelCombination> ModelCombinationList;
typedef std::set<ModelCombination> ModelCombinationSet;

/// Maps a 'combined system' to the functionality it can 'theoretically'
/// provide when looking at its resources
typedef std::map<ModelCombination, owlapi::model::IRIList> Combination2FunctionMap;
typedef std::map<owlapi::model::IRI, ModelCombinationSet > Function2CombinationMap;

class FunctionalityMapping
{
    ModelPool mModelPool;
    owlapi::model::IRIList mServices;
    ModelPool mFunctionalSaturationBound;

    Combination2FunctionMap mCombination2Function;
    Function2CombinationMap mFunction2Combination;

public:
    FunctionalityMapping();

    FunctionalityMapping(const ModelPool& modelPool,
        const owlapi::model::IRIList& services,
        const ModelPool& functionalSaturationBound);

    const owlapi::model::IRIList& getFunction(const ModelCombination& combination) const;
    const ModelCombinationSet& getCombinations(const owlapi::model::IRI& iri) const;

    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }
    const ModelPool& getModelPool() const { return mModelPool; }

    void setFunctionalSaturationBound(const ModelPool& bounds) { mFunctionalSaturationBound = bounds; }
    const ModelPool& getFunctionalSaturationBound() const { return mFunctionalSaturationBound; }

    void addFunction(const ModelCombination& combination, const owlapi::model::IRIList& functionModels);

    void add(const ModelCombination& combination, const owlapi::model::IRIList& functionModels);

    std::string toString() const;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FUNCTIONALITY_MAPPING_HPP
