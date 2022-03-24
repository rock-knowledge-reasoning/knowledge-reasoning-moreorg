#ifndef MOREORG_POLICIES_ALL_SELECTION_HPP
#define MOREORG_POLICIES_ALL_SELECTION_HPP

#include "SelectionPolicy.hpp"

namespace moreorg {
namespace policies {

class AllSelection : public SelectionPolicy
{
public:
    AllSelection() = default;
    virtual ~AllSelection() = default;

    Selection apply(const Selection& agent,
                    const OrganizationModelAsk& ask) const override;
};

} // end namespace policies
} // end namespace moreorg
#endif // MOREORG_POLICIES_ALL_SELECTION_HPP
