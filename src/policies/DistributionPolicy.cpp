#include "DistributionPolicy.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace policies {

DistributionPolicy::DistributionPolicy(const IRI& iri)
    : Policy(iri)
{}

DistributionPolicy::DistributionPolicy(const ModelPool& pool,
       const OrganizationModelAsk& ask,
       const IRI& iri)
    : Policy(pool, ask, iri)
{}


} // end namespace policies
} // end namespace moreorg
