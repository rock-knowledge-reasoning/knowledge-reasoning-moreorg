#ifndef ORGANIZATION_MODEL_PROPERTY_CONSTRAINT_SOLVER_HPP
#define ORGANIZATION_MODEL_PROPERTY_CONSTRAINT_SOLVER_HPP

#include <gecode/search.hh>
#include <gecode/float.hh>
#include "PropertyConstraint.hpp"
#include "ValueBound.hpp"

namespace organization_model {

/**
 *
 */
class PropertyConstraintSolver : public Gecode::Space
{
public:
    PropertyConstraintSolver();

    PropertyConstraintSolver(PropertyConstraintSolver& other);

    virtual ~PropertyConstraintSolver();

    virtual Gecode::Space* copy(void);

    /**
     * Merge a list of constraints a returns the corresponding allowed value bound
     * \return ValueBound The allowed value range
     * \throws std::invalid_argument when constraints cannot be fulfilled
     */
    static ValueBound merge(const PropertyConstraint::List& constraints);
    /**
     * Merge a list of constraints a returns the corresponding allowed value bound
     * \return ValueBound The allowed value range
     * \throws std::invalid_argument when constraints cannot be fulfilled
     */
    static ValueBound merge(const PropertyConstraint::Set& constraints);

private:
    Gecode::FloatVar mValue;

};

} // end namespace templ
#endif // ORGANIZATION_MODEL_PROPERTY_CONSTRAINT_SOLVER_HPP
