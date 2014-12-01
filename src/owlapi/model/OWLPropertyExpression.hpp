#ifndef OWL_API_MODEL_OWL_PROPERTY_EXPRESSION_HPP
#define OWL_API_MODEL_OWL_PROPERTY_EXPRESSION_HPP

#include <boost/shared_ptr.hpp>
#include <owl_om/owlapi/model/OWLObject.hpp>

namespace owlapi {
namespace model {

/**
 *  Expressions represent complex notions in the domain being described. For example, a class expression describes a set of individuals in terms of the restrictions on the individuals' characteristics. 
 */
class OWLPropertyExpression : public OWLObject
{
public:
    typedef boost::shared_ptr<OWLPropertyExpression> Ptr;

    virtual bool isDataPropertyExpression() const { throw std::runtime_error("OWLPropertyExpression::isDataPropertyExpression: not implemented"); }
    virtual bool isObjectPropertyExpression() const { throw std::runtime_error("OWLPropertyExpression::isObjectPropertyExpression: not implemented"); }

    virtual std::string toString() const { throw std::runtime_error("OWLPropertyExpression::toString: not implemented"); }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_PROPERTY_EXPRESSION_HPP
