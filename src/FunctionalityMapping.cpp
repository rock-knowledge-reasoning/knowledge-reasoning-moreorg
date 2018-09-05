#include "FunctionalityMapping.hpp"
#include <sstream>
#include <algorithm>
#include "Algebra.hpp"

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
        mFunction2Pool[*cit] = ModelPool::Set();
    }
}

const ModelPool::Set& FunctionalityMapping::getModelPools(const owlapi::model::IRI& iri) const
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

owlapi::model::IRIList FunctionalityMapping::getFunctionalities(const ModelPool& pool) const
{
    owlapi::model::IRIList functions;
    for(const Function2PoolMap::value_type& p : mFunction2Pool)
    {
        bool supported = false;
        for(const ModelPool& fPool : p.second)
        {
            ModelPoolDelta delta = Algebra::substract(fPool, pool);
            if(Algebra::isSubset(fPool, pool))
            {
                supported = true;
                break;
            }
        }
        if(supported)
        {
            functions.push_back(p.first);
        }
    }
    return functions;
}

void FunctionalityMapping::add(const ModelPool& modelPool, const owlapi::model::IRI& function)
{
    if(!modelPool.empty())
    {
        mFunction2Pool[function].insert(modelPool);
        mSupportedFunctionalities.insert(function);
        mActiveModelPools.insert(modelPool);
    }
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


std::string FunctionalityMapping::toString(uint32_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ss << hspace << "FunctionalityMapping:" << std::endl;
    ss << mModelPool.toString(indent) << std::endl;
    ss << hspace << "FunctionalSaturationBound -- ";
    ss << mFunctionalSaturationBound.toString(indent) << std::endl;

    {
        Function2PoolMap::const_iterator cit = mFunction2Pool.begin();
        ss << hspace << "Function --> Pool:" << std::endl;
        for(; cit != mFunction2Pool.end(); ++cit)
        {
            ss << hspace << "   - function: " << cit->first.toString() << std::endl;
            ss << hspace << "     pools: " << std::endl;

            const ModelPool::Set& modelPoolSet = cit->second;
            ss << ModelPool::toString(modelPoolSet, indent + 8) << std::endl;
        }
    }

    return ss.str();
}

} // end namespace organization_model
