#ifndef OWLAPI_MODEL_OWL_OBJECT_PROPERTY_EXPRESSION_HPP
#define OWLAPI_MODEL_OWL_OBJECT_PROPERTY_EXPRESSION_HPP

#include <stdexcept>
#include <owl_om/owlapi/model/OWLPropertyExpression.hpp>

namespace owlapi {
namespace model {

/**
 * \details Object properties can by used in OWL 2 to form object property expressions, which represent relationships between pairs of individuals. They are represented in the structural specification of OWL 2 by ObjectPropertyExpression
 */
class OWLObjectPropertyExpression : public OWLPropertyExpression
{
public:
    typedef boost::shared_ptr< OWLObjectPropertyExpression > Ptr;

    /**
     * Get the named property used in this property expression
     */
    OWLObjectPropertyExpression::Ptr getNamedProperty() const { throw std::runtime_error("not implemented"); }

    /**
     * Return property in its simplified form
     */
    OWLObjectPropertyExpression::Ptr getSimplified() const { throw std::runtime_error("not implemented"); }

    bool isDataPropertyExpression() const { return false; }
    bool isObjectPropertyExpression() const { return true; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_OBJECT_PROPERTY_EXPRESSION_HPP
