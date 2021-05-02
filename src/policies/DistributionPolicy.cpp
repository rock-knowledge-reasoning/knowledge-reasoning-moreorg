#include "DistributionPolicy.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace policies {

DistributionPolicy::DistributionPolicy(const IRI& iri)
    : Policy(iri)
{}

Distribution DistributionPolicy::apply(const ModelPool& modelPool,
        const OrganizationModelAsk& ask) const
{

    return Distribution();
}

} // end namespace policies
} // end namespace moreorg
