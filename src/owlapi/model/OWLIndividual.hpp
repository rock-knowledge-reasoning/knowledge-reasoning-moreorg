#ifndef OWLAPI_MODEL_OWL_INDIVIDUAL_HPP
#define OWLAPI_MODEL_OWL_INDIVIDUAL_HPP

#include <boost/shared_ptr.hpp>
#include <owl_om/owlapi/model/OWLObject.hpp>

namespace owlapi {
namespace model {

/**
 * \class OWLIndividual
 * \brief Represents a named or anonymous individual
 */
class OWLIndividual : public OWLObject //, OWLPropertyAssertionObject
{
public:
    typedef boost::shared_ptr<OWLIndividual> Ptr;
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_INDIVIDUAL_HPP
