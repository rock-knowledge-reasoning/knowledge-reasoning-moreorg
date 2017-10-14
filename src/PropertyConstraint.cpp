#include "PropertyConstraint.hpp"

namespace organization_model {

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

} // end namespace organization_model
