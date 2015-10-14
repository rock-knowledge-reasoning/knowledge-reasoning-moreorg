#ifndef ORGANIZATION_MODEL_ALGEBRA_HPP
#define ORGANIZATION_MODEL_ALGEBRA_HPP

#include <organization_model/OrganizationModel.hpp>

namespace organization_model {

class Algebra
{
public:
    /**
     * Compute delta as b - a
     */
    static ModelPoolDelta delta(const ModelPoolDelta& a, const ModelPoolDelta& b) { return substract(a,b); }

    /**
     * Substract a from b
     */
    static ModelPoolDelta substract(const ModelPoolDelta& a, const ModelPoolDelta& b);
    static ModelPoolDelta sum(const ModelPoolDelta& a, const ModelPoolDelta& b);
    /**
     * Merge two systems, i.e. when they are identical M o M = M, M o N = M+N
     */
    static ModelPool merge(const ModelPool& a, const ModelPool& b);

    /**
     * Compute the maximum of each entry
     */
    static ModelPool max(const ModelPool& a, const ModelPool& b);

    /**
     *
     * Compute the minimum of each entry
     */
    static ModelPool min(const ModelPool& a, const ModelPool& b);

    static ModelPool merge(const ModelCombinationSet& a, const ModelCombination& b);

    static ModelPool merge(const std::set<ModelPool>& pool);
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_HPP
