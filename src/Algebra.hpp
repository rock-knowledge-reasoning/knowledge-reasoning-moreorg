#ifndef ORGANIZATION_MODEL_ALGEBRA_HPP
#define ORGANIZATION_MODEL_ALGEBRA_HPP

#include "OrganizationModel.hpp"

namespace moreorg {

class Algebra
{
public:
    enum OperationType { UNKNOWN, SUM_OP, MIN_OP, MAX_OP, OPERATION_END };
    /**
     * Compute delta as b - a
     */
    static ModelPoolDelta delta(const ModelPoolDelta& a, const ModelPoolDelta& b) { return substract(a,b); }

    /**
     * Substract a from b, i.e. b-a
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
    static ModelPool max(const ModelPool::List& modelPoolList);

    /**
     * Compute the minimum of each entry of two given ModelPool instances
     * \return ModelPool that contains the minimum cardinalities
     */
    static ModelPool min(const ModelPool& a, const ModelPool& b);

    /**
     * Compute the minimum of each entry for a list of ModelPool instances
     * \return ModelPool that contains the minimum cardinalities
     */
    static ModelPool min(const ModelPool::List& modelPoolList);

    static ModelPool merge(const ModelCombinationSet& a, const ModelCombination& b);

    static ModelPool merge(const std::set<ModelPool>& modelPoolSet);

    /**
     * Multiply the cardinalities with a given factor
     */
    static ModelPool multiply(const ModelPool& pool, uint32_t factor);

    /**
     * Apply cartesian product like:
     * a = {(0,1),(1,1),(2,0)}
     * b = {(0,0),(1,2),(2,0)}
     *
     * a [max] b = {(0,1),(1,2),(2,0)}
     */
    static ModelPool::Set maxCompositions(const ModelPool& a, const ModelPool& b);

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
    static ModelPool::Set maxCompositions(const ModelPool::Set& a, const ModelPool::Set& b);

    /**
     * Test if a is a subset of b, i.e., a equal b or b includes a.
     * By definition are empty set subsets of all sets
     \f[
         a \subseteq b
     \f]
     * \return True if a is a subset of b, false otherwise
     */
    static bool isSubset(const ModelPool& a, const ModelPool& b);

    /**
     * Test if a is a superset of b, i.e., if a includes b or a equals b
     * By definition all set are superset for an empty set
     \f[
         a \supseteq b
     \f]
     * \return True if a is a superset of b, false otherwise
     */
    static bool isSuperset(const ModelPool& a, const ModelPool& b);
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_ALGEBRA_HPP
