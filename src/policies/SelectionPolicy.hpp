#ifndef ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP
#define ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP

#include "../Policy.hpp"

namespace moreorg {
namespace policies {

struct Selection
{
    // The selection
    ModelPool agent;
    // Value characterising this modelPool
    double value;
};

class SelectionPolicy : public Policy
{
public:
    typedef shared_ptr<SelectionPolicy> Ptr;

    SelectionPolicy(const owlapi::model::IRI& iri = owlapi::model::IRI());

    SelectionPolicy(const ModelPool& pool,
           const OrganizationModelAsk& ask,
           const owlapi::model::IRI& iri);

    virtual ~SelectionPolicy() = default;

    virtual const Selection& getSelection() const { return mSelection; }

protected:
    Selection mSelection;
};

} // policies
} // moreorg

#endif // ORGANIZATION_MODEL_POLICIES_SELECTION_POLICY_HPP
