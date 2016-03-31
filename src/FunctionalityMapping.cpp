#include "FunctionalityMapping.hpp"
#include <sstream>
#include <algorithm>

namespace organization_model {

FunctionalityMapping::FunctionalityMapping()
{}

FunctionalityMapping::FunctionalityMapping(const ModelPool& modelPool,
        const owlapi::model::IRIList& functionalities,
        const ModelPool& functionalSaturationBound)
    : mModelPool(modelPool)
    , mFunctionalities(functionalities)
    , mFunctionalSaturationBound(functionalSaturationBound)
{
    owlapi::model::IRIList::const_iterator cit = mFunctionalities.begin();
    for(; cit != mFunctionalities.end(); ++cit)
    {
        // initialize the set of functionalities
        mFunction2Pool[*cit] = ModelPoolSet();
    }
}

const owlapi::model::IRIList& FunctionalityMapping::getFunction(const ModelPool& modelPool) const
{
    ModelPool boundedPool = modelPool.applyUpperBound(mFunctionalSaturationBound);
    Pool2FunctionMap::const_iterator cit = mPool2Function.find(boundedPool);
    if(cit != mPool2Function.end())
    {
        return cit->second;
    } else {
        throw std::invalid_argument("organization_model::FunctionalityMapping::getFunction: could not find"
                " model pool: " + boundedPool.toString());
    }
}

const ModelPoolSet& FunctionalityMapping::getModelPools(const owlapi::model::IRI& iri) const
{
    Function2PoolMap::const_iterator cit = mFunction2Pool.find(iri);
    if(cit != mFunction2Pool.end())
    {
        return cit->second;
    } else {
        throw std::invalid_argument("organization_model::FunctionalityMapping::getModelPools: could not find"
                " model pools with function: " + iri.toString());
    }
}

void FunctionalityMapping::addFunction(const ModelPool& modelPool,
        const owlapi::model::IRI& function)
{
    owlapi::model::IRIList& functions = mPool2Function[modelPool];
    owlapi::model::IRIList::const_iterator cit = std::find(functions.begin(), functions.end(), function);
    if(cit != functions.end())
    {
        mPool2Function[modelPool].push_back(function);
    }
}

void FunctionalityMapping::addFunction(const ModelPool& modelPool,
        const owlapi::model::IRIList& functionModels)
{
    owlapi::model::IRIList::const_iterator cit = functionModels.begin();
    for(; cit != functionModels.end(); ++cit)
    {
        addFunction(modelPool, *cit);
    }
}

void FunctionalityMapping::add(const ModelPool& modelPool, const owlapi::model::IRI& functionModel)
{
    addFunction(modelPool, functionModel);
    mFunction2Pool[functionModel].insert(modelPool);
}

void FunctionalityMapping::add(const ModelPool& modelPool, const owlapi::model::IRIList& functionModels)
{
    using namespace owlapi::model;
    IRIList::const_iterator cit = functionModels.begin();
    for(; cit != functionModels.end(); ++cit)
    {
        IRI functionModel = *cit;
        add(modelPool, functionModel);
    }
}


std::string FunctionalityMapping::toString() const
{
    std::stringstream ss;
    ss << "FunctionalityMapping:" << std::endl;
    ss << mModelPool.toString() << std::endl;
    ss << "FunctionalSaturationBound -- ";
    ss << mFunctionalSaturationBound.toString() << std::endl;

    {
        Pool2FunctionMap::const_iterator cit = mPool2Function.begin();
        ss << "Pool --> Functions:" << std::endl;
        for(; cit != mPool2Function.end(); ++cit)
        {
            ss << "   - pool: " << owlapi::model::IRI::toString((cit->first).toModelCombination(), true) << std::endl;
            ss << "     function: " << owlapi::model::IRI::toString(cit->second, true) << std::endl;
        }
    }
    {
        Function2PoolMap::const_iterator cit = mFunction2Pool.begin();
        ss << "Function --> Pool:" << std::endl;
        for(; cit != mFunction2Pool.end(); ++cit)
        {
            ss << "   - function: " << cit->first.toString() << std::endl;
            ss << "     pools: " << std::endl;
            const ModelPoolSet& modelPoolSet = cit->second;
            ss << "        " << ModelPool::toString(cit->second) << std::endl;
        }
    }

    return ss.str();
}

} // end namespace organization_model
