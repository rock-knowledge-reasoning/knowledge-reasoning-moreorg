#ifndef MOREORG_POLICIES_PROPERTY_BASED_SELECTION_HPP
#define MOREORG_POLICIES_PROPERTY_BASED_SELECTION_HPP

#include "SelectionPolicy.hpp"

namespace moreorg {
namespace policies {

class PropertyBasedSelection : public SelectionPolicy
{
public:
    PropertyBasedSelection() = default;

    PropertyBasedSelection(const owlapi::model::IRI& property,
            const owlapi::model::IRI& op);

    virtual ~PropertyBasedSelection() = default;

    policies::Selection apply(const policies::Selection& selection,
            const OrganizationModelAsk& ask) const override;

protected:
    owlapi::model::IRI mProperty;
    owlapi::model::IRI mOperator;

};

} // end namespace policies
} // end namespace moreorg
#endif // MOREORG_POLICIES_PROPERTY_BASED_SELECTION_HPP
