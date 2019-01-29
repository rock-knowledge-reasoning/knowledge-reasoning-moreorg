#ifndef ORGANIZATION_MODEL_QUERY_CACHE_HPP
#define ORGANIZATION_MODEL_QUERY_CACHE_HPP

#include <tuple>
#include <unordered_map>
#include <functional>
#include <boost/functional/hash.hpp>
#include <owlapi/OWLApi.hpp>
#include "ModelPool.hpp"

namespace std {
using namespace owlapi::model;
using namespace organization_model;
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
} // end namespace std

namespace organization_model {

class QueryCache
{
public:
    typedef std::tuple<ModelPool, owlapi::model::IRI,
        owlapi::model::OWLCardinalityRestriction::OperationType,
        bool> CRQuery;

    typedef std::unordered_map< CRQuery, owlapi::model::OWLCardinalityRestriction::PtrList> CRQueryResults;

    std::pair<owlapi::model::OWLCardinalityRestriction::PtrList, bool> getCachedResult(const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty,
        owlapi::model::OWLCardinalityRestriction::OperationType operationType,
        bool max2Min) const;

    void cacheResult(const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty,
        owlapi::model::OWLCardinalityRestriction::OperationType operationType,
        bool max2Min,
        const owlapi::model::OWLCardinalityRestriction::PtrList& list);

protected:
    // From propery key
    CRQueryResults mQueryResults;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_QUERY_CACHE_HPP
