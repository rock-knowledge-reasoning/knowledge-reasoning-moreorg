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

} // end namespace organization_model
