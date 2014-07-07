#ifndef OWL_API_MODEL_OWL_NAMED_INDIVIDUAL_HPP
#define OWL_API_MODEL_OWL_NAMED_INDIVIDUAL_HPP

#include <owl_om/owlapi/model/OWLEntity.hpp>
#include <owl_om/owlapi/model/entities/OWLLogicalEntity.hpp>

namespace owlapi {
namespace model {

class OWLNamedIndividual : public OWLIndividual, OWLLogicalEntity
{
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_NAMED_INDIVIDUAL_HPP
