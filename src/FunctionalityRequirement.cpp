#include "FunctionalityRequirement.hpp"
#include <algorithm>
#include <base-logging/Logging.hpp>
#include <sstream>

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

std::string FunctionalityRequirement::toString(size_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ss << hspace << mFunctionality.getModel().toQuotedString() << std::endl;
    ss << hspace << "    Constraints:" << std::endl;
    for(const PropertyConstraint& constraint : mPropertyConstraints)
    {
        ss << hspace << "        " << constraint.toString();
    }
    return ss.str();
}

std::string FunctionalityRequirement::toString(const FunctionalityRequirement::Map& requirements, size_t indent)
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    for(const FunctionalityRequirement::Map::value_type& r : requirements)
    {
        ss << hspace << r.first.toString() << std::endl;
        ss << r.second.toString(indent + 4) << std::endl;
    }
    return ss.str();
}

}
