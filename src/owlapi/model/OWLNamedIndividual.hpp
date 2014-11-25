#ifndef OWLAPI_MODEL_OWL_NAMED_INDIVIDUAL_HPP
#define OWLAPI_MODEL_OWL_NAMED_INDIVIDUAL_HPP

#include <boost/shared_ptr.hpp>
#include <owl_om/owlapi/model/OWLIndividual.hpp>
#include <owl_om/owlapi/model/OWLLogicalEntity.hpp>

namespace owlapi {
namespace model {

class OWLNamedIndividual : public OWLIndividual, public OWLLogicalEntity
{
public:
    typedef boost::shared_ptr<OWLNamedIndividual> Ptr;

    OWLNamedIndividual( const IRI& iri)
        : OWLLogicalEntity(iri)
    {}
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_NAMED_INDIVIDUAL_HPP
