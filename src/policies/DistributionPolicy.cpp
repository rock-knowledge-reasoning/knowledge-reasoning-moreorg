#include "DistributionPolicy.hpp"
#include "../Agent.hpp"
#include "../vocabularies/OM.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace policies {

DistributionPolicy::DistributionPolicy(const IRI& iri,
                                       const OrganizationModelAsk& ask)
    : Policy(iri)
    , mAsk(ask)
{
    {
        OWLAnnotationValue::Ptr annotationValue =
            ask.ontology().getAnnotationValue(
                iri,
                vocabulary::OM::resolve("hasAtomicAgentRule"));
        IRI atomicAgentRule = annotationValue->asIRI();
        mAtomicAgentRule =
            InferenceRule::loadAtomicAgentRule(atomicAgentRule, ask);
    }

    {
        OWLAnnotationValue::Ptr annotationValue =
            ask.ontology().getAnnotationValue(
                iri,
                vocabulary::OM::resolve("hasCompositeAgentRule"));
        IRI compositeAgentRule = annotationValue->asIRI();
        mCompositeAgentRule =
            InferenceRule::loadCompositeAgentRule(compositeAgentRule, ask);
    }
}

Distribution DistributionPolicy::apply(const ModelPool& modelPool,
                                       const OrganizationModelAsk& ask) const
{
    Distribution distribution;
    if(modelPool.numberOfInstances() == 1)
    {
        distribution.shares[modelPool.begin()->first] = 1.0;
        return distribution;
    }

    facades::Robot robot = facades::Robot::getInstance(modelPool, ask);
    double denom = mCompositeAgentRule->apply(robot);

    for(const std::pair<IRI, size_t>& p : modelPool)
    {
        ModelPool atomicAgent;
        atomicAgent[p.first] = 1;

        facades::Robot aa = facades::Robot::getInstance(atomicAgent, ask);
        double value = mAtomicAgentRule->apply(aa);

        distribution.shares[p.first] = value / denom;
    }

    return distribution;
}

} // end namespace policies
} // end namespace moreorg
