#ifndef OWL_API_MODEL_OWL_OBJECT_PROPERTY_HPP
#define OWL_API_MODEL_OWL_OBJECT_PROPERTY_HPP

#include <boost/shared_ptr.hpp>
#include <owl_om/owlapi/model/OWLProperty.hpp>
#include <owl_om/owlapi/model/OWLObjectPropertyExpression.hpp>

namespace owlapi {
namespace model {

class OWLObjectProperty : public OWLObjectPropertyExpression, public OWLProperty
{
public:
    typedef boost::shared_ptr<OWLObjectProperty> Ptr;

    OWLObjectProperty(const IRI& iri)
        : OWLProperty(iri)
    {}

    bool isDataPropertyExpression() const { return false; }
    bool isObjectPropertyExpression() const { return true; }

    /**
     * Convert OWLObjectProperty to a string -- based on the underlying iri
     */
    std::string toString() const { return getIRI().toString(); }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_OBJECT_PROPERTY_HPP
