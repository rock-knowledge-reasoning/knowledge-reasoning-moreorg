#include "PropertyConstraint.hpp"
#include <sstream>
#include <tuple>

namespace organization_model {

std::map<PropertyConstraint::ConstraintType, std::string> PropertyConstraint::TypeTxt = {
    {PropertyConstraint::UNKNOWN, "UNKNOWN"},
    {PropertyConstraint::EQUAL, "=="},
    {PropertyConstraint::LESS_THAN, "<"},
    {PropertyConstraint::LESS_EQUAL, "<="},
    {PropertyConstraint::GREATER_EQUAL, ">="},
    {PropertyConstraint::GREATER_THEN, ">"},
    {CONSTRAINT_TYPE_END, "ConstraintTypeEnd"}
    }
    ;

PropertyConstraint::PropertyConstraint()
    : mType(UNKNOWN)
{}

PropertyConstraint::PropertyConstraint(const owlapi::model::IRI& dataProperty,
        ConstraintType c,
        double value)
    : mType(c)
    , mDataProperty(dataProperty)
    , mValue(value)
{
}

std::string PropertyConstraint::toString() const
{
    std::stringstream ss;
    ss << mDataProperty.toString() << " " << TypeTxt[mType] << " " << mValue;
    return ss.str();
}

bool PropertyConstraint::operator==(const PropertyConstraint& other) const
{
    return other.mType == mType && other.mDataProperty == mDataProperty && other.mValue == mValue;
}

bool PropertyConstraint::operator<(const PropertyConstraint& other) const
{
    if(*this == other)
    {
        return false;
    }

    if(mType < other.mType)
    {
        return true;
    } else if(mType == other.mType)
    {
        if(mDataProperty < other.mDataProperty)
        {
            return true;
        } else if(mDataProperty == other.mDataProperty)
        {
            return mValue < other.mValue;
        }
    }
    return false;
}

PropertyConstraint::Clusters PropertyConstraint::getClusters(const PropertyConstraint::Set& constraints)
{
    std::map<owlapi::model::IRI, PropertyConstraint::Set> clustered;
    for(const PropertyConstraint& constraint : constraints)
    {
        clustered[constraint.getProperty()].insert( constraint );
    }
    return clustered;
}

} // end namespace organization_model
