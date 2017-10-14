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

    const Functionality& getFunctionality() const { return mFunctionality; }

    const PropertyConstraint::List& getPropertyConstraints() const { return mPropertyConstraints; }

private:
    Functionality mFunctionality;
    PropertyConstraint::List mPropertyConstraints;
};

} // namespace organization_model
#endif // ORGANIZATION_MODEL_FUNCTIONALITY_REQUIREMENT_HPP
