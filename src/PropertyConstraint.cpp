#include "PropertyConstraint.hpp"
#include <sstream>
#include <tuple>
#include "facades/Robot.hpp"

namespace organization_model {

std::map<PropertyConstraint::ConstraintType, std::string> PropertyConstraint::TypeTxt = {
    {PropertyConstraint::UNKNOWN, "UNKNOWN"},
    {PropertyConstraint::EQUAL, "=="},
    {PropertyConstraint::LESS_THAN, "<"},
    {PropertyConstraint::LESS_EQUAL, "<="},
    {PropertyConstraint::GREATER_EQUAL, ">="},
    {PropertyConstraint::GREATER_THAN, ">"},
    {CONSTRAINT_TYPE_END, "ConstraintTypeEnd"}
    }
    ;

PropertyConstraint::PropertyConstraint()
    : mType(UNKNOWN)
    , mRValProperty()
{}

PropertyConstraint::PropertyConstraint(const owlapi::model::IRI& dataProperty,
        ConstraintType c,
        double value)
    : mType(c)
    , mDataProperty(dataProperty)
    , mValue(value)
    , mRValProperty()
{
}

PropertyConstraint::PropertyConstraint(const owlapi::model::IRI& dataProperty,
        ConstraintType c,
        const owlapi::model::IRI& rvalProperty)
    : mType(c)
    , mDataProperty(dataProperty)
    , mValue()
    , mRValProperty(rvalProperty)
{
}

std::string PropertyConstraint::toString() const
{
    std::stringstream ss;
    if(!mRValProperty.empty())
    {
        ss << mDataProperty.toString() << " " << TypeTxt[mType] << " " << mRValProperty.toString();
    } else {
        ss << mDataProperty.toString() << " " << TypeTxt[mType] << " " << mValue;
    }
    return ss.str();
}

std::string PropertyConstraint::toString(const PropertyConstraint::List& list, size_t indent)
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    for(const PropertyConstraint& pc : list)
    {
        ss << pc.toString() << std::endl;
    }
    return ss.str();
}

std::string PropertyConstraint::toString(const PropertyConstraint::Set& set, size_t indent)
{
    PropertyConstraint::List list(set.begin(), set.end());
    return toString(list, indent);
}

bool PropertyConstraint::operator==(const PropertyConstraint& other) const
{
    return other.mType == mType && other.mDataProperty == mDataProperty && other.mValue == mValue
        && other.mRValProperty == mRValProperty;
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
            if(mRValProperty < other.mRValProperty)
            {
                return true;
            } else {
                return mValue < other.mValue;
            }
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

double PropertyConstraint::getReferenceValue(const facades::Robot& robot) const
{
    if(usesPropertyReference())
    {
        return robot.getPropertyValue(mRValProperty);
    }
    return mValue;
}

} // end namespace organization_model
