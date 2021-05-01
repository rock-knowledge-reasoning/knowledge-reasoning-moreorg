#ifndef MOREORG_POLICIES_FUNCTIONALITY_BASED_SELECTION_HPP
#define MOREORG_POLICIES_FUNCTIONALITY_BASED_SELECTION_HPP

#include "SelectionPolicy.hpp"

namespace moreorg {
namespace policies {

class FunctionalityBasedSelection : public SelectionPolicy
{
public:
    FunctionalityBasedSelection(const owlapi::model::IRI& functionality =
            owlapi::model::IRI());

    virtual ~FunctionalityBasedSelection() = default;

    /**
     *
     */
    policies::Selection apply(const policies::Selection& agent,
            const OrganizationModelAsk& ask) const override;

protected:
    owlapi::model::IRI mFunctionality;

};

} // end namespace policies
} // end namespace moreorg
#endif // MOREORG_POLICIES_FUNCTIONALITY_BASED_SELECTION_HPP
