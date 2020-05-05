#ifndef ORGANIZATION_MODEL_REASONING_MODEL_BOUND_HPP
#define ORGANIZATION_MODEL_REASONING_MODEL_BOUND_HPP

#include <vector>
#include <map>
#include <owlapi/model/IRI.hpp>

namespace moreorg {
namespace reasoning {

/**
 * ModelBound represents
 * the allowed interval for cardinalities of a particular model
 */
struct ModelBound
{
    typedef std::vector<ModelBound> List;

    static uint32_t MAX_THRESHOLD;

    owlapi::model::IRI model;

    /// lower bound
    uint32_t min;
    /// upper bound
    uint32_t max;

    ModelBound();
    ModelBound(const owlapi::model::IRI& model, uint32_t min, uint32_t max);

    static uint32_t getMaxResourceCount(const List& boundList);
    std::string toString(size_t indent = 0) const;
    static std::string toString(const List& boundList, size_t indent = 0);

    /**
     * Substract the given ModelBound from the current object
     * this->max - other.min
     * \throws invalid_argument if model in other does not match to this
     * \throws invalid_argument if other.min > this->max
     */
    ModelBound substractMin(const ModelBound& other) const;

    /**
     * Substract min values of list b from a max and(!) min values
     * \param a List of model bounds (lval)
     * \param b List of model bounds (rval)
     * \param removeNegative Remove results with a negative result
     * \returns a - b
     */
    static ModelBound::List substractMin(const ModelBound::List& a, const ModelBound::List& b, bool removeNegative = false);

    /**
     * Decrement min and max values
     * \throws std::runtime_error if null values exist and cannot be further decremented
     */
    void decrement();

    bool operator<(const ModelBound& other) const;
};

} // end namespace reasoning
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_REASONING_MODEL_BOUND_HPP
