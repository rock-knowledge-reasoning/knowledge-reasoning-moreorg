#ifndef OWLAPI_MODEL_OWL_UNARY_PROPERTY_AXIOM_HPP
#define OWLAPI_MODEL_OWL_UNARY_PROPERTY_AXIOM_HPP

#include <owl_om/owlapi/model/OWLAxiom.hpp>

namespace owlapi {
namespace model {

class OWLUnaryPropertyAxiom
{
public:
    typedef boost::shared_ptr<OWLUnaryPropertyAxiom> Ptr;

    virtual OWLPropertyExpression::Ptr getProperty() = 0;
}

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_UNARY_PROPERTY_AXIOM_HPP
