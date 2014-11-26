#ifndef OWLAPI_MODEL_OWL_NARY_AXIOM_HPP
#define OWLAPI_MODEL_OWL_NARY_AXIOM_HPP

#include <owl_om/owlapi/model/OWLAxiom.hpp>

namespace owlapi {
namespace model {

/**
 * Represents an interface definition for an axiom that contains two or more operands that could also be represented with multiple pairwise axioms
 */
class OWLNaryAxiom
{
public:
    typedef boost::shared_ptr<OWLNaryAxiom> Ptr;

    virtual std::vector<OWLAxiom::Ptr> asPairwiseAxioms() = 0;
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_NARY_AXIOM_HPP
