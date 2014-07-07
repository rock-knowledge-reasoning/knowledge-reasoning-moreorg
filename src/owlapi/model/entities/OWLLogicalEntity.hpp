#ifndef OWL_API_MODEL_LOGICAL_ENTITY_HPP
#define OWL_API_MODEL_LOGICAL_ENTITY_HPP

#include <owl_om/owlapi/model/OWLEntity.hpp>

namespace owlapi {
namespace model {

/**
 * \class OWLLogicalEntity
 * \brief All entities that are not an OWLAnnotationProperty
 */
class OWLLogicalEntity : public OWLEntity
{
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_LOGICAL_ENTITY_HPP
