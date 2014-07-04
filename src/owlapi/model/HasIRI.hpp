#ifndef OWL_API_MODEL_HAS_IRI_HPP
#define OWL_API_MODEL_HAS_IRI_HPP

#include <owl_om/Exceptions.hpp>
#include <owl_om/owlapi/model/IRI.hpp>

namespace owlapi {
namespace model {

class HasIRI
{
public:
    virtual IRI getIRI() const { throw owl_om::NotImplemented("owlapi::model::HasIRI::getIRI()"); }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_HAS_IRI_HPP


