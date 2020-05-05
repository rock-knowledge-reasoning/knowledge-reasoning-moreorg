#include "QueryCache.hpp"

namespace moreorg {

std::pair<owlapi::model::OWLCardinalityRestriction::PtrList, bool> QueryCache::getCachedResult(const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty,
        owlapi::model::OWLCardinalityRestriction::OperationType operationType,
        bool max2Min) const
{
    using namespace owlapi::model;
    std::pair<owlapi::model::OWLCardinalityRestriction::PtrList, bool> result;
    CRQuery query = std::make_tuple(modelPool, objectProperty, operationType, max2Min);
    CRQueryResults::const_iterator cit = mQueryResults.find(query);
    if(cit == mQueryResults.end())
    {
        result.second = false;
    } else {
        result.first = cit->second;
        result.second = true;
    }
    return result;
}

void QueryCache::cacheResult(const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty,
        owlapi::model::OWLCardinalityRestriction::OperationType operationType,
        bool max2Min,
        const owlapi::model::OWLCardinalityRestriction::PtrList& list)
{
    CRQuery query = std::make_tuple(modelPool, objectProperty, operationType, max2Min);
    mQueryResults.emplace(query, list);
}

std::pair<ModelPool::List, bool> QueryCache::getCachedResult(const ModelPool& modelPool,
        const Resource::Set& r) const
{
    std::pair<ModelPool::List, bool> result;
    CoalitionStructureQuery query = std::make_tuple(modelPool, r);
    CSQueryResults::const_iterator cit = mCSQueryResults.find(query);
    if(cit == mCSQueryResults.end())
    {
        result.second = false;
    } else {
        result.first = cit->second;
        result.second = true;
    }
    return result;
}

void QueryCache::cacheResult(const ModelPool& modelPool,
        const Resource::Set& r,
        const ModelPool::List& list)
{
    CoalitionStructureQuery query = std::make_tuple(modelPool, r);
    mCSQueryResults.emplace(query, list);
}

void QueryCache::clear()
{
    mQueryResults.clear();
    mCSQueryResults.clear();
}

} // end namespace moreorg


