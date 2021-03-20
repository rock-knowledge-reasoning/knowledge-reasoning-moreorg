#ifndef ORGANIZATION_MODEL_POLICY_HPP
#define ORGANIZATION_MODEL_POLICY_HPP

#include "OrganizationModelAsk.hpp"

namespace moreorg {

/**
 * Policy class permits to encapsulate and apply distribution and selection rules
 *
 * SelectionPolicy: permits to pick one of more atomic agents, that fulfill a subsystem critera
 * DistributionPolicy: compute a shared for each atomic agent forming the general agent
 */
class Policy
{
public:
    typedef shared_ptr<Policy> Ptr;
    typedef std::function<Policy::Ptr(const ModelPool&, const OrganizationModelAsk&)> CreationFunc;

    Policy(const owlapi::model::IRI& iri = owlapi::model::IRI());

    Policy(const ModelPool& pool,
           const OrganizationModelAsk& ask,
           const owlapi::model::IRI& iri = owlapi::model::IRI());

    virtual ~Policy();

    virtual const owlapi::model::IRI& getIRI() const { return mIRI; }

    std::map<owlapi::model::IRI, double> computeSharesByType(const
            owlapi::model::IRI& property,
            const ModelPool& pool,
            const OrganizationModelAsk& ask
            ) const;

    /**
     * Update the policy object
     */
    virtual void update(const ModelPool& pool, const OrganizationModelAsk& ask) = 0;

    static Policy::Ptr getInstance(const owlapi::model::IRI& policyName,
            const ModelPool& pool,
            const OrganizationModelAsk& ask);

    static void registerCreationFuncs(const owlapi::model::IRI& policyName,
            CreationFunc func);

    template<typename T>
    static Policy::Ptr create(const ModelPool& pool, const OrganizationModelAsk& ask)
    {
        return make_shared<T>(pool, ask);
    }

protected:
    owlapi::model::IRI mIRI;
    ModelPool mModelPool;
    OrganizationModelAsk mAsk;

    static std::map<owlapi::model::IRI, CreationFunc > msCreationFuncs;
    static std::map<owlapi::model::IRI, std::map<ModelPool, Policy::Ptr> > msPolicies;
};

template<typename T>
class PolicyRegistration
{
public:
    PolicyRegistration()
    {
        registerPolicy();
    }

    void registerPolicy()
    {
        T policy;
        Policy::CreationFunc f = &Policy::create<T>;
        Policy::registerCreationFuncs(policy.getIRI(),f);
    }
};


} // end namespace moreorg
#endif // ORGANIZATION_MODEL_POLICY_HPP
