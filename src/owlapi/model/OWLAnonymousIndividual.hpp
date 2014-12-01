#ifndef OWL_API_MODEL_ANONYMOUS_INDIVIDUAL_HPP
#define OWL_API_MODEL_ANONYMOUS_INDIVIDUAL_HPP

#include <owl_om/owlapi/model/NodeID.hpp>
#include <owl_om/owlapi/model/OWLObject.hpp>

namespace owlapi {
namespace model {

/**
 * \class OWLAnonymousIndividual
 * \brief Represents Anonymous Individuals in the OWL 2 Specification
 * \see http://www.w3.org/TR/2009/REC-owl2-syntax-20091027/#Anonymous_Individuals
 */
class OWLAnonymousIndividual : public OWLIndividual //, OWLAnnotationValue, OWLAnnotationSubject
{
    NodeID mNodeID;
public:
    OWLAnonymousIndividual();
    OWLAnonymousIndividual(const NodeID& nodeId)
        : mNodeID()
    {}

    NodeID getID() const { return mNodeID; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_ANONYMOUS_INDIVIDUAL_HPP
