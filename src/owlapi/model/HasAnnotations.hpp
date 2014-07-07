#ifndef OWL_API_MODEL_HAS_ANNOTATIONS_HPP
#define OWL_API_MODEL_HAS_ANNOTATIONS_HPP

#include <boost/shared_ptr.hpp>
#include <vector>

namespace owlapi {
namespace model {

class OWLAnnotation;
typedef boost::shared_ptr<OWLAnnotation> OWLAnnotationPtr;
typedef std::vector<OWLAnnotationPtr> OWLAnnotationPtrList;

class HasAnnotations
{
protected:
    OWLAnnotationPtrList mAnnotations;

public:
    OWLAnnotationPtrList getAnnotations() const { return mAnnotations; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_HAS_ANNOTATIONS_HPP
