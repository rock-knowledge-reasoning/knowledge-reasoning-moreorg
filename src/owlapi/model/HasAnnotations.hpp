#ifndef OWLAPI_MODEL_HAS_ANNOTATIONS_HPP
#define OWLAPI_MODEL_HAS_ANNOTATIONS_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

#include <owl_om/owlapi/model/OWLAnnotation.hpp>

namespace owlapi {
namespace model {

class OWLAnnotation;
typedef std::vector<OWLAnnotation> OWLAnnotationList;

class HasAnnotations
{
protected:
    OWLAnnotationList mAnnotations;

public:
    HasAnnotations(OWLAnnotationList annotations)
        : mAnnotations(annotations)
    {}

    OWLAnnotationList getAnnotations() const { return mAnnotations; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_HAS_ANNOTATIONS_HPP
