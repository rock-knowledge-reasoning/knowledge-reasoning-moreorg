#include "PropertyConstraint.hpp"
#include <sstream>

namespace organization_model {

std::map<PropertyConstraint::ConstraintType, std::string> PropertyConstraint::TypeTxt = {
    {PropertyConstraint::UNKNOWN, "UNKNOWN"},
    {PropertyConstraint::EQ, "=="},
    {PropertyConstraint::LT, "<"},
    {PropertyConstraint::LE, "<="},
    {PropertyConstraint::GE, ">="},
    {PropertyConstraint::GT, ">"},
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
    if(mType < other.mType)
    {
        return true;
    } else if(mDataProperty < other.mDataProperty)
    {
        return true;
    } else {
        return mValue < other.mValue;
    }
    return false;
}

} // end namespace organization_model
