#include "Algebra.hpp"

namespace organization_model {

ModelPoolDelta Algebra::delta(const ModelPoolDelta& a, const ModelPoolDelta& b)
{
    ModelPoolDelta delta;

    ModelPoolDelta::const_iterator ait = a.begin();
    for(; ait != a.end(); ++ait)
    {
        owlapi::model::IRI model = ait->first;
        size_t modelCount = ait->second;

        ModelPoolDelta::const_iterator bit = b.find(model);
        if(bit != b.end())
        {
            delta[model] = bit->second - modelCount;
        } else {
            delta[model] = 0 - modelCount;
        }
    }

    ModelPoolDelta::const_iterator bit = b.begin();
    for(; bit != b.end(); ++bit)
    {
        owlapi::model::IRI model = bit->first;
        size_t modelCount = bit->second;

        ModelPoolDelta::const_iterator ait = a.find(model);
        if(ait != a.end())
        {
            // already handled
        } else {
            delta[model] = modelCount;
        }
    }

    return delta;
}

ModelPoolDelta Algebra::sum(const ModelPoolDelta& a, const ModelPoolDelta& b)
{
    ModelPoolDelta sum = a;
    ModelPoolDelta::const_iterator cit = b.begin();
    for(; cit != b.end(); ++cit)
    {
        sum[cit->first] += cit->second;
    }
    return sum;
}

ModelPool Algebra::merge(const ModelPool& a, const ModelPool& b)
{
    std::set<ModelPool> modelPoolSet;
    modelPoolSet.insert(a);
    modelPoolSet.insert(b);

    return merge(modelPoolSet);
}

ModelPool Algebra::merge(const std::set<ModelCombination>& a, const ModelCombination& b)
{
    // Create a set
    std::set<ModelPool> modelPoolSet;

    // Convert combination to pool
    std::set<ModelCombination>::const_iterator cit = a.begin();
    for(; cit != a.end(); ++cit)
    {
        modelPoolSet.insert( OrganizationModel::combination2ModelPool(*cit) );

    }
    modelPoolSet.insert( OrganizationModel::combination2ModelPool(b) );

    return merge(modelPoolSet);
}

ModelPool Algebra::max(const ModelPool& a, const ModelPool& b)
{
    ModelPool resultPool = a;
    ModelPool::const_iterator bit = b.begin();
    for(; bit != b.end(); ++bit)
    {
        const owlapi::model::IRI& model = bit->first;
        uint32_t cardinality = bit->second;

        ModelPool::iterator rit = resultPool.find(model);
        if(rit != resultPool.end())
        {
            rit->second = std::max( bit->second, rit->second );
        } else {
            resultPool.insert(ModelPool::value_type(model, cardinality));
        }

    }
    return resultPool;
}

ModelPool Algebra::min(const ModelPool& a, const ModelPool& b)
{
    ModelPool resultPool = a;
    ModelPool::const_iterator bit = b.begin();
    for(; bit != b.end(); ++bit)
    {
        const owlapi::model::IRI& model = bit->first;
        uint32_t cardinality = bit->second;

        ModelPool::iterator rit = resultPool.find(model);
        if(rit != resultPool.end())
        {
            rit->second = std::min( bit->second, rit->second );
        } else {
            resultPool.insert(ModelPool::value_type(model, cardinality));
        }

    }
    return resultPool;
}

ModelPool Algebra::merge(const std::set<ModelPool>& modelPoolSet)
{
    std::set<ModelPool>::const_iterator cit = modelPoolSet.begin();
    bool init = true;
    ModelPoolDelta mergedPool;
    for(; cit != modelPoolSet.end(); ++cit)
    {
        if(init)
        {
            mergedPool = *cit;
            init = false;
        } else {
            mergedPool = sum(mergedPool, *cit);
        }
    }

    return mergedPool.toModelPool();
}

} // end namespace organization_model
