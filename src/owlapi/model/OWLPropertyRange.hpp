#ifndef OWLAPI_MODEL_OWL_PROPERTY_RANGE_HPP
#define OWLAPI_MODEL_OWL_PROPERTY_RANGE_HPP

#include <owl_om/owlapi/model/OWLObject.hpp>

namespace owlapi {
namespace model {

/**
 * \class OWLPropertyRange
 * \brief Emptry class serving as marker interface for objects that can be the ranges of properties
 */
class OWLPropertyRange : public OWLObject//, SWRLPredicate
{
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_PROPERTY_RANGE_HPP
