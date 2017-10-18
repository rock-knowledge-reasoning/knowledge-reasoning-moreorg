#include "PropertyConstraintSolver.hpp"
#include <gecode/minimodel.hh>
#include <base-logging/Logging.hpp>

namespace organization_model {

PropertyConstraintSolver::PropertyConstraintSolver()
    : Gecode::Space()
    , mValue(*this, Gecode::Float::Limits::min, Gecode::Float::Limits::max)
{}

PropertyConstraintSolver::PropertyConstraintSolver(bool share, PropertyConstraintSolver& other)
    : Gecode::Space(share, other)
{
    mValue.update(*this, share, other.mValue);
}

PropertyConstraintSolver::~PropertyConstraintSolver()
{}

Gecode::Space* PropertyConstraintSolver::copy(bool share)
{
    return new PropertyConstraintSolver(share, *this);
}

ValueBound PropertyConstraintSolver::merge(const PropertyConstraint::List& constraints)
{
    PropertyConstraint::Set constraintSet(constraints.begin(), constraints.end());
    return merge(constraintSet);
}

ValueBound PropertyConstraintSolver::merge(const PropertyConstraint::Set& constraints)
{
    PropertyConstraintSolver propertyConstraintSolver;
    LOG_WARN_S << "Start merge: ";

    for(const PropertyConstraint& constraint : constraints)
    {
        LOG_WARN_S << "Adding constraint: " << constraint.toString();
        double value = constraint.getValue();
        switch(constraint.getType())
        {
            case PropertyConstraint::EQUAL:
                Gecode::rel(propertyConstraintSolver, propertyConstraintSolver.mValue,Gecode::FRT_EQ, value);
                break;
            case PropertyConstraint::LESS_EQUAL:
                Gecode::rel(propertyConstraintSolver, propertyConstraintSolver.mValue,Gecode::FRT_LQ, value);
                break;
            case PropertyConstraint::LESS_THAN:
                Gecode::rel(propertyConstraintSolver, propertyConstraintSolver.mValue,Gecode::FRT_LE, value);
                break;
            case PropertyConstraint::GREATER_EQUAL:
                Gecode::rel(propertyConstraintSolver, propertyConstraintSolver.mValue,Gecode::FRT_GQ, value);
                break;
            case PropertyConstraint::GREATER_THEN:
                Gecode::rel(propertyConstraintSolver, propertyConstraintSolver.mValue,Gecode::FRT_GR, value);
                break;
            default:
                break;
        }
    }

    propertyConstraintSolver.status();
    if(propertyConstraintSolver.failed())
    {
        throw std::invalid_argument("organization_model::PropertyConstraintSolver: constraints can not be fulfilled");
    } else {
        ValueBound valueBound(propertyConstraintSolver.mValue.min(), propertyConstraintSolver.mValue.max());
        return valueBound;
    }
}

} // end namespace templ
