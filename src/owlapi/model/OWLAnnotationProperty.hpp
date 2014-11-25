#ifndef OWLAPI_MODEL_OWL_ANNOTATION_PROPERTY_HPP
#define OWLAPI_MODEL_OWL_ANNOTATION_PROPERTY_HPP

#include <owl_om/owlapi/model/OWLProperty.hpp>

namespace owlapi {
namespace model {

class OWLAnnotationProperty : public OWLProperty
{
    /**
     * Determines if this annotation property has an IRI corresponding to
     * {@code rdfs:comment}
     */
    bool isComment() { throw std::runtime_error("OWLAnnotationProperty::isComment: not implemented"); }

    /**
     * Determines if this annotation property has an IRI corresponding to
      {@code rdfs:label}.
     */
    bool isLabel() { throw std::runtime_error("OWLAnnotationProperty::isLabel: not implemented"); }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_ANNOTATION_PROPERTY_HPP
