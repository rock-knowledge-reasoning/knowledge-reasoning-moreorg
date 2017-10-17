#ifndef ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP
#define ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP

#include <vector>
#include <set>
#include <owlapi/model/IRI.hpp>

namespace organization_model {

class PropertyConstraint
{
public:
    typedef std::vector<PropertyConstraint> List;
    typedef std::set<PropertyConstraint> Set;

    enum ConstraintType { UNKNOWN, EQ, LT, LE, GE, GT, CONSTRAINT_TYPE_END };

    static std::map<ConstraintType, std::string> TypeTxt;

    PropertyConstraint();

    PropertyConstraint(const owlapi::model::IRI& dataProperty, ConstraintType c, double value);

    ConstraintType getType() const { return mType; }

    const owlapi::model::IRI& getProperty() const { return mDataProperty; }

    double getValue() const { return mValue; }

    std::string toString() const;

    bool operator==(const PropertyConstraint& other) const;
    bool operator<(const PropertyConstraint& other) const;

private:
    ConstraintType mType;
    owlapi::model::IRI mDataProperty;
    double mValue;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP
