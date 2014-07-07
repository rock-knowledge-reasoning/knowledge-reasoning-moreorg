#ifndef OWL_API_MODEL_OWL_ANNOTATION_HPP
#define OWL_API_MODEL_OWL_ANNOTATION_HPP

#include <owl_om/owlapi/model/HasAnnotations.hpp>
#include <owl_om/owlapi/model/OWLAnnotationProperty.hpp>
#include <owl_om/owlapi/model/OWLAnnotationValue.hpp>

namespace owlapi {
namespace model {

class OWLAnnotation : public HasAnnotations
{
    OWLAnnotationProperty mProperty;
    OWLAnnotationValue mValue;

public:

};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_ANNOTATION_HPP
