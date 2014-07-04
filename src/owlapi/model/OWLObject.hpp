#ifndef OWL_API_MODEL_OBJECT_HPP
#define OWL_API_MODEL_OBJECT_HPP

#include <owl_om/Exceptions.hpp>

namespace owlapi {
namespace model {

class OWLObject
{
public:
    virtual bool isBottomEntity() { throw owl_om::NotImplemented("owlapi::model::OWLObject::isBottomEntity"); }
    virtual bool isTopEntity() { throw owl_om::NotImplemented("owlapi::model::OWLObject::isTopEntity"); }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OBJECT_HPP
