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

    PropertyConstraintSolver(bool share, PropertyConstraintSolver& other);

    virtual ~PropertyConstraintSolver();

    virtual Gecode::Space* copy(bool share);

    static ValueBound merge(const PropertyConstraint::List& constraints);
    static ValueBound merge(const PropertyConstraint::Set& constraints);

private:
    Gecode::FloatVar mValue;

};

} // end namespace templ
#endif // ORGANIZATION_MODEL_PROPERTY_CONSTRAINT_SOLVER_HPP
