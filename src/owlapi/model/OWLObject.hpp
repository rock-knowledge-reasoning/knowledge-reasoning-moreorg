#ifndef OWLAPI_MODEL_OWL_OBJECT_HPP
#define OWLAPI_MODEL_OWL_OBJECT_HPP

#include <owl_om/Exceptions.hpp>
#include <boost/shared_ptr.hpp>

namespace owlapi {
namespace model {

class OWLAxiomVisitor;

class OWLObject
{
public:
    typedef boost::shared_ptr<OWLObject> Ptr;

    /**
     * Accept a visitor to visit this object, i.e. 
     * will call visitor->visit(*this)
     */
    virtual void accept(boost::shared_ptr<OWLAxiomVisitor> visitor);

    virtual bool isBottomEntity();
    virtual bool isTopEntity();
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_OBJECT_HPP
