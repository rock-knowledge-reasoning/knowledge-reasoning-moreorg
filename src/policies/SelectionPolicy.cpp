#include "SelectionPolicy.hpp"
#include "../Agent.hpp"
#include <base-logging/Logging.hpp>

using namespace owlapi::model;

namespace moreorg {
namespace policies {

SelectionPolicy::SelectionPolicy(const IRI& iri)
    : Policy(iri)
{}

Selection SelectionPolicy::apply(const Selection& agentSelection,
        const OrganizationModelAsk& ask) const
{
    Selection selection = agentSelection;
    for(const SelectionPolicy::Ptr& sp : mPolicyChain)
    {
        selection = sp->apply(selection, ask);
    }
    return selection;
}

} // end namespace policies
} // end namespace moreorg
