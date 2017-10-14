#ifndef ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP
#define ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP

#include <vector>
#include <owlapi/model/IRI.hpp>

namespace organization_model {

class PropertyConstraint
{
public:
    typedef std::vector<PropertyConstraint> List;

    enum ConstraintType { UNKNOWN, EQ, LT, LE, GE, GT, CONSTRAINT_TYPE_END };

    PropertyConstraint();

    PropertyConstraint(const owlapi::model::IRI& dataProperty, ConstraintType c, double value);

    ConstraintType getType() const { return mType; }

    const owlapi::model::IRI& getProperty() const { return mDataProperty; }

    double getValue() const { return mValue; }

private:
    ConstraintType mType;
    owlapi::model::IRI mDataProperty;
    double mValue;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP
