#ifndef ORGANIZATION_MODEL_QUERY_CACHE_HPP
#define ORGANIZATION_MODEL_QUERY_CACHE_HPP

#include <tuple>
#include <unordered_map>
#include <functional>
#include <boost/functional/hash.hpp>
#include <owlapi/OWLApi.hpp>
#include "ModelPool.hpp"
#include "Resource.hpp"

namespace std {
using namespace owlapi::model;
using namespace moreorg;
template<>
struct hash< tuple<ModelPool, IRI, OWLCardinalityRestriction::OperationType, bool> >
{
    size_t operator()(const tuple<ModelPool, IRI,
            OWLCardinalityRestriction::OperationType, bool>& tpl) const
    {
        size_t seed = 0;
        for(const ModelPool::value_type& v : get<0>(tpl))
        {
            boost::hash_combine(seed, v.first.toString());
            boost::hash_combine(seed, v.second);
        }
        boost::hash_combine(seed, get<1>(tpl).toString());
        boost::hash_combine(seed, get<2>(tpl));
        boost::hash_combine(seed, get<3>(tpl));
        return seed;
    }
};

template<>
struct hash< tuple<ModelPool, Resource::Set> >
{
    size_t operator()(const tuple<ModelPool, Resource::Set>& tpl) const
    {
        size_t seed = 0;
        for(const ModelPool::value_type& v : get<0>(tpl))
        {
            boost::hash_combine(seed, v.first.toString());
            boost::hash_combine(seed, v.second);
        }
        for(const Resource& r : get<1>(tpl))
        {
            boost::hash_combine(seed, r.getModel().toString());
        }
        return seed;
    }
};
} // end namespace std

namespace moreorg {

class QueryCache
{
public:
    typedef std::tuple<ModelPool, owlapi::model::IRI,
        owlapi::model::OWLCardinalityRestriction::OperationType,
        bool> CRQuery;

    typedef std::tuple<ModelPool, Resource::Set> CoalitionStructureQuery;


    /// Cardinality Restriction Query Results
    typedef std::unordered_map< CRQuery, owlapi::model::OWLCardinalityRestriction::PtrList> CRQueryResults;
    /// Coalition Structure Query Results
    typedef std::unordered_map< CoalitionStructureQuery, ModelPool::List> CSQueryResults;

    std::pair<owlapi::model::OWLCardinalityRestriction::PtrList, bool> getCachedResult(const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty,
        owlapi::model::OWLCardinalityRestriction::OperationType operationType,
        bool max2Min) const;

    void cacheResult(const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty,
        owlapi::model::OWLCardinalityRestriction::OperationType operationType,
        bool max2Min,
        const owlapi::model::OWLCardinalityRestriction::PtrList& list);

    std::pair<ModelPool::List, bool> getCachedResult(const ModelPool& modelPool,
            const Resource::Set& r) const;

    void cacheResult(const ModelPool& modelPool,
            const Resource::Set& r,
            const ModelPool::List& list);

    void clear();
protected:
    // From propery key
    CRQueryResults mQueryResults;
    CSQueryResults mCSQueryResults;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_QUERY_CACHE_HPP
