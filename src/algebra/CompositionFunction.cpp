#include "CompositionFunction.hpp"
#include <limits>

namespace moreorg {
namespace algebra {

double CompositionFunction::weightedSum(const ModelPool& modelPool,
                                        const IRIValueMap& valueMap)
{
    double value = 0.0;
    for(const ModelPool::value_type& p : modelPool)
    {
        IRIValueMap::const_iterator cit = valueMap.find(p.first);
        if(cit != valueMap.end())
        {
            value += p.second * cit->second;
        } else
        {
            throw std::invalid_argument(
                "moreorg::algebra::CompositionFunction::weightedSum"
                " no value found for '" +
                p.first.toString() + "'");
        }
    }
    return value;
}

double CompositionFunction::min(const ModelPool& modelPool,
                                const IRIValueMap& valueMap)
{
    double value = std::numeric_limits<double>::max();
    for(const ModelPool::value_type& p : modelPool)
    {
        IRIValueMap::const_iterator cit = valueMap.find(p.first);
        if(cit != valueMap.end())
        {
            double propertyValue = cit->second;
            if(propertyValue < value)
            {
                value = propertyValue;
            }
        } else
        {
            throw std::invalid_argument(
                "moreorg::algebra::CompositionFunction::max"
                " no value found for '" +
                p.first.toString() + "'");
        }
    }
    return value;
}

double CompositionFunction::max(const ModelPool& modelPool,
                                const IRIValueMap& valueMap)
{
    double value = std::numeric_limits<double>::min();
    for(const ModelPool::value_type& p : modelPool)
    {
        IRIValueMap::const_iterator cit = valueMap.find(p.first);
        if(cit != valueMap.end())
        {
            double propertyValue = cit->second;
            if(propertyValue > value)
            {
                value = propertyValue;
            }
        } else
        {
            throw std::invalid_argument(
                "moreorg::algebra::CompositionFunction::max"
                " no value found for '" +
                p.first.toString() + "'");
        }
    }
    return value;
}

} // end namespace algebra
} // end namespace moreorg
