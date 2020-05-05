#ifndef ORGANIZATION_MODEL_ALGEBRA_COMPOSITION_FUNCTION_HPP
#define ORGANIZATION_MODEL_ALGEBRA_COMPOSITION_FUNCTION_HPP

#include "../SharedPtr.hpp"
#include <map>
#include <owlapi/model/IRI.hpp>
#include "../ModelPool.hpp"

namespace moreorg {
namespace algebra {

typedef std::map<owlapi::model::IRI, double> IRIValueMap;

/// Allow the composition of multiple values into a single
typedef function<double(const ModelPool&, const IRIValueMap&)> CompositionFunc;

/**
 * Class wrapping the default composition functions for a composite agent property, which is
 * derived from the atomic agents properties
 */
class CompositionFunction
{
public:
    static double weightedSum(const ModelPool& modelPool, const IRIValueMap& valueMap);
    static double min(const ModelPool& modelPool, const IRIValueMap& valueMap);
    static double max(const ModelPool& modelPool, const IRIValueMap& valueMap);

};

} // end namespace algebra
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_ALGEBRA_COMPOSITION_FUNCTION_HPP
