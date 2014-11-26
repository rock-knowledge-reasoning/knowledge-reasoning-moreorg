#ifndef OWLAPI_MODEL_OWL_PROPERTY_ASSERTION_OBJECT_HPP
#define OWLAPI_MODEL_OWL_PROPERTY_ASSERTION_OBJECT_HPP

#include <owl_om/owlapi/model/OWLObject.hpp>

namespace owlapi {
namespace model {

/**
 * A marker interface for the types of property assertion objects (individuals and literals) that are the objects of property assertions. 
 */
class OWLPropertyAssertionObject : public OWLObject
{
public:
    typedef boost::shared_ptr<OWLPropertyAssertionObject> Ptr;
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_PROPERTY_ASSERTION_OBJECT_HPP
