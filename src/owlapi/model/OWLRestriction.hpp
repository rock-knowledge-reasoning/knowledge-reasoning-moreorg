#ifndef OWL_API_MODEL_OWL_RESTRICTION_HPP
#define OWL_API_MODEL_OWL_RESTRICTION_HPP

#include <owl_om/owlapi/model/OWLClassExpression.hpp>
#include <owl_om/owlapi/model/OWLPropertyExpression.hpp>

namespace owlapi {
namespace model {

/**
 * \class OWLRestriction
 * \brief Represents a general restriction
 * \see http://owlapi.sourceforge.net/javadoc/index.html
 */
class OWLRestriction : public OWLClassExpression
{
    OWLPropertyExpression::Ptr mpProperty;

public: 
    OWLRestriction(OWLPropertyExpression::Ptr property)
        : mpProperty(property)
    {}

    virtual OWLPropertyExpression::Ptr getProperty() const { return mpProperty; }

    virtual bool isDataRestriction() const { return  mpProperty->isDataPropertyExpression(); }
    virtual bool isObjectRestriction() const { return mpProperty->isObjectPropertyExpression(); }

};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_RESTRICTION_HPP
