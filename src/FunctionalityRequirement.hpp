#ifndef ORGANIZATION_MODEL_FUNCTIONALITY_REQUIREMENT_HPP
#define ORGANIZATION_MODEL_FUNCTIONALITY_REQUIREMENT_HPP

#include <map>
#include "Functionality.hpp"
#include "PropertyConstraint.hpp"

namespace organization_model {

class FunctionalityRequirement
{
public:
    typedef std::map<Functionality, FunctionalityRequirement> Map;

    FunctionalityRequirement();

    FunctionalityRequirement(const Functionality& functionality,
            const PropertyConstraint::List& propertyConstraints);

    FunctionalityRequirement(const Functionality& functionality,
            const PropertyConstraint::Set& propertyConstraints);

    const Functionality& getFunctionality() const { return mFunctionality; }

    const PropertyConstraint::Set& getPropertyConstraints() const { return mPropertyConstraints; }

    void addPropertyConstraint(const PropertyConstraint& constraint);

    /**
     * Add a list of property constraints
     */
    void addPropertyConstraints(const PropertyConstraint::List& constraints);
    void addPropertyConstraints(const PropertyConstraint::Set& constraints);

    std::string toString(size_t indent = 0) const;
    static std::string toString(const FunctionalityRequirement::Map& requirements, size_t indent = 0);

private:
    Functionality mFunctionality;
    PropertyConstraint::Set mPropertyConstraints;
};

} // namespace organization_model
#endif // ORGANIZATION_MODEL_FUNCTIONALITY_REQUIREMENT_HPP
