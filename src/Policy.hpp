#ifndef ORGANIZATION_MODEL_POLICY_HPP
#define ORGANIZATION_MODEL_POLICY_HPP

#include "OrganizationModelAsk.hpp"

namespace moreorg {

/**
 * Policy class permits to encapsulate and apply distribution and selection
 * rules
 *
 * \see SelectionPolicy: permits to pick one of more atomic agents, that fulfill
 * a subsystem critera \see DistributionPolicy: compute a shared for each atomic
 * agent forming the general agent
 */
class Policy
{
public:
    typedef shared_ptr<Policy> Ptr;
    typedef owlapi::model::IRI Id;

    static const size_t MAX_POLICY_ELEMENTS = 100;

    Policy(const owlapi::model::IRI& iri = owlapi::model::IRI());

    virtual ~Policy();

    virtual const owlapi::model::IRI& getIRI() const { return mIRI; }

    std::map<owlapi::model::IRI, double>
    computeSharesByType(const owlapi::model::IRI& property,
                        const ModelPool& pool,
                        const OrganizationModelAsk& ask) const;

    static Policy::Ptr getInstance(const owlapi::model::IRI& policyName,
                                   const OrganizationModelAsk& ask);

protected:
    owlapi::model::IRI mIRI;
    static std::map<owlapi::model::IRI, Policy::Ptr> msPolicies;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_POLICY_HPP
