#include "FunctionalityRequirement.hpp"

namespace organization_model {

FunctionalityRequirement::FunctionalityRequirement()
    : mFunctionality(owlapi::model::IRI())
{}

FunctionalityRequirement::FunctionalityRequirement(const Functionality& functionality,
        const PropertyConstraint::List& propertyConstraints)
    : mFunctionality(functionality)
    , mPropertyConstraints(propertyConstraints)
{
}

}
