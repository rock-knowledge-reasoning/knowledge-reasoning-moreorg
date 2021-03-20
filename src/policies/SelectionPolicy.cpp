#include "SelectionPolicy.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace policies {

SelectionPolicy::SelectionPolicy(const IRI& iri)
    : Policy(iri)
{}

SelectionPolicy::SelectionPolicy(const ModelPool& pool,
       const OrganizationModelAsk& ask,
       const IRI& iri)
    : Policy(pool, ask, iri)
{}


} // end namespace policies
} // end namespace moreorg
