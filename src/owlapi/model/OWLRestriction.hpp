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
    OWLPropertyExpression mProperty;

public: 
    OWLRestriction(const OWLPropertyExpression& property)
        : mProperty(property)
    {}

    virtual OWLPropertyExpression getProperty() const { return mProperty; }

    virtual bool isDataRestriction() const { return  mProperty.isDataProperty(); }
    virtual bool isObjectRestriction() const { return mProperty.isObjectProperty(); }

};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_RESTRICTION_HPP
