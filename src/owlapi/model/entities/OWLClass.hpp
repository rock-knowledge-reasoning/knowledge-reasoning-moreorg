#ifndef OWL_API_MODEL_OWL_CLASS_HPP
#define OWL_API_MODEL_OWL_CLASS_HPP

#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLNamedObject.hpp>
#include <owl_om/owlapi/model/entities/OWLLogicalEntity.hpp>

namespace owlapi {
namespace model {

class OWLClass : public OWLClassExpression, OWLLogicalEntity, OWLNamedObject
{
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_CLASS_HPP
