#include "ModelPool.hpp"

namespace organization_model {

ModelPoolDelta::ModelPoolDelta()
{}

ModelPoolDelta::ModelPoolDelta(const ModelPool& pool)
{
    ModelPool::const_iterator cit = pool.begin();
    for(; cit != pool.end(); ++cit)
    {
        insert(std::pair<owlapi::model::IRI, int>(cit->first, cit->second));
    }
}

bool ModelPoolDelta::isNegative() const
{
    ModelPoolDelta::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(cit->second < 0)
        {
            return true;
        }
    }
    return false;
}

} // end namespace organization_model
