#ifndef OWL_API_MODEL_OBJECT_HPP
#define OWL_API_MODEL_OBJECT_HPP

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
#endif // OWL_API_MODEL_OBJECT_HPP
