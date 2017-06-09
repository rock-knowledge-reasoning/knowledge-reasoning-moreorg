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
     * Compute the maximum of each entry of two given ModelPool instances
     * \return ModelPool that contains the maximum cardinalities
     */
    static ModelPool max(const ModelPool& a, const ModelPool& b);

    /**
     * Compute the maximum of each entry for a list of ModelPool instances
     * \return ModelPool that contains the maximum cardinalities
     */
    static ModelPool max(const ModelPoolList& modelPoolList);

    /**
     * Compute the minimum of each entry of two given ModelPool instances
     * \return ModelPool that contains the minimum cardinalities
     */
    static ModelPool min(const ModelPool& a, const ModelPool& b);

    /**
     * Compute the minimum of each entry for a list of ModelPool instances
     * \return ModelPool that contains the minimum cardinalities
     */
    static ModelPool min(const ModelPoolList& modelPoolList);

    static ModelPool merge(const ModelCombinationSet& a, const ModelCombination& b);

    static ModelPool merge(const std::set<ModelPool>& pool);

    /**
     * Apply cartesian product like:
     * a = {(0,1),(1,1),(2,0)}
     * b = {(0,0),(1,2),(2,0)}
     *
     * a [max] b = {(0,1),(1,2),(2,0)}
     */
    static ModelPoolSet maxCompositions(const ModelPool& a, const ModelPool& b);

    /**
     * Apply cartesian product like
     *
     * a = {{(0,1),(1,1),(2,0)}, {(0,3)} }
     * b = {(0,0),(1,2),(2,0)}
     *
     * but use the max function to compute the resulting model pool
     *
     * We define this as [max] operator
     *
     * a [max] b = { {(0,3),(1,1),(2,0)}, {(0,3),(1,2),(2,0)} }
     */
    static ModelPoolSet maxCompositions(const ModelPoolSet& a, const ModelPoolSet& b);
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_HPP
