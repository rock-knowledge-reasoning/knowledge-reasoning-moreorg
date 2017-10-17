#include "FunctionalityRequirement.hpp"
#include <algorithm>
#include <base-logging/Logging.hpp>

namespace organization_model {

FunctionalityRequirement::FunctionalityRequirement()
    : mFunctionality(owlapi::model::IRI())
{}

FunctionalityRequirement::FunctionalityRequirement(const Functionality& functionality,
        const PropertyConstraint::Set& propertyConstraints)
    : mFunctionality(functionality)
    , mPropertyConstraints(propertyConstraints)
{
}

FunctionalityRequirement::FunctionalityRequirement(const Functionality& functionality,
        const PropertyConstraint::List& propertyConstraints)
    : mFunctionality(functionality)
    , mPropertyConstraints(propertyConstraints.begin(), propertyConstraints.end())
{
}

void FunctionalityRequirement::addPropertyConstraint(const PropertyConstraint& constraint)
{
    mPropertyConstraints.insert(constraint);
}

void FunctionalityRequirement::addPropertyConstraints(const PropertyConstraint::List& constraints)
{
    for(const PropertyConstraint& c : constraints)
    {
        addPropertyConstraint(c);
    }
}

void FunctionalityRequirement::addPropertyConstraints(const PropertyConstraint::Set& constraints)
{
    for(const PropertyConstraint& c : constraints)
    {
        addPropertyConstraint(c);
    }
}

}
