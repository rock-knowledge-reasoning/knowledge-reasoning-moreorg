#ifndef OWL_API_MODEL_OWL_ANNOTATION_HPP
#define OWL_API_MODEL_OWL_ANNOTATION_HPP

#include <vector>
#include <owl_om/owlapi/model/OWLObject.hpp>
#include <owl_om/owlapi/model/OWLAnnotationProperty.hpp>
#include <owl_om/owlapi/model/OWLAnnotationValue.hpp>

namespace owlapi {
namespace model {

class OWLAnnotation : public OWLObject
{
    OWLAnnotationProperty mProperty;
    OWLAnnotationValue mValue;

public:

};

typedef std::vector<OWLAnnotation> OWLAnnotationList;

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_ANNOTATION_HPP
