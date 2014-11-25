#ifndef OWLAPI_MODEL_OWL_PROPERTY_HPP
#define OWLAPI_MODEL_OWL_PROPERTY_HPP

#include <boost/shared_ptr.hpp>
#include <owl_om/owlapi/model/OWLPropertyExpression.hpp>
#include <owl_om/owlapi/model/OWLLogicalEntity.hpp>

namespace owlapi {
namespace model {

/**
 * \details A marker interface for properties that aren't expressions i.e. named
 * properties. By definition, properties are either data properties or object
 * properties.
 */
class OWLProperty : public OWLPropertyExpression, public OWLLogicalEntity
{
public:
    typedef boost::shared_ptr<OWLProperty> Ptr;

    OWLProperty(const IRI& iri)
        : OWLLogicalEntity(iri)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_PROPERTY_HPP
