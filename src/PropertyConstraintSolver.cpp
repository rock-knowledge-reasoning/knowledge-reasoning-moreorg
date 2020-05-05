#include "PropertyConstraintSolver.hpp"
#include <gecode/minimodel.hh>
#include <base-logging/Logging.hpp>
#include "facades/Robot.hpp"

namespace moreorg {

PropertyConstraintSolver::PropertyConstraintSolver()
    : Gecode::Space()
{}

PropertyConstraintSolver::PropertyConstraintSolver(PropertyConstraintSolver& other)
    : Gecode::Space(other)
{
    std::map<owlapi::model::IRI, Gecode::FloatVar>::iterator it =
        other.mValues.begin();
    for(; it != other.mValues.end(); ++it)
    {
        Gecode::FloatVar& var = mValues[it->first];
        var.update(*this, it->second);
    }
}

PropertyConstraintSolver::~PropertyConstraintSolver()
{}

Gecode::Space* PropertyConstraintSolver::copy()
{
    return new PropertyConstraintSolver(*this);
}

ValueBound PropertyConstraintSolver::merge(const PropertyConstraint::List& constraints)
{
    PropertyConstraint::Set constraintSet(constraints.begin(), constraints.end());
    return merge(constraintSet);
}

ValueBound PropertyConstraintSolver::merge(const PropertyConstraint::Set& constraints)
{
    PropertyConstraintSolver propertyConstraintSolver;

    for(const PropertyConstraint& constraint : constraints)
    {
        Gecode::FloatVar& var = propertyConstraintSolver.getVariable(constraint.getProperty());
        double value = constraint.getValue();

        switch(constraint.getType())
        {
            case PropertyConstraint::EQUAL:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_EQ, value);
                break;
            case PropertyConstraint::LESS_EQUAL:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_LQ, value);
                break;
            case PropertyConstraint::LESS_THAN:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_LE, value);
                break;
            case PropertyConstraint::GREATER_EQUAL:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_GQ, value);
                break;
            case PropertyConstraint::GREATER_THAN:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_GR, value);
                break;
            default:
                break;
        }
    }

    propertyConstraintSolver.status();
    if(propertyConstraintSolver.failed())
    {
        throw std::invalid_argument("moreorg::PropertyConstraintSolver: constraints cannot be fulfilled");
    } else {
        double minValue = 0;
        double maxValue = Gecode::Float::Limits::max;
        for(const std::map<owlapi::model::IRI, Gecode::FloatVar>::value_type& p
                : propertyConstraintSolver.mValues)
        {
            if(minValue < p.second.min())
            {
                minValue = p.second.min();
            }
            if(maxValue > p.second.max())
            {
                maxValue = p.second.max();
            }
        }
        if(minValue > maxValue)
        {
            throw
                std::invalid_argument("moreorg::PropertyConstraintSolver:"
                        " constraints cannot be fulfilled - required min >"
                        " required max");
        }
        ValueBound valueBound(minValue, maxValue);
        return valueBound;
    }
}

Fulfillment PropertyConstraintSolver::fulfills(const facades::Robot& robot,
        const PropertyConstraint::List& constraints)
{
    PropertyConstraint::Set constraintSet(constraints.begin(), constraints.end());
    return fulfills(robot, constraintSet);
}

Fulfillment PropertyConstraintSolver::fulfills(const facades::Robot& robot,
        const PropertyConstraint::Set& constraints)
{
    PropertyConstraintSolver propertyConstraintSolver;

    PropertyConstraint::Set conflicts;
    for(const PropertyConstraint& constraint : constraints)
    {
        Gecode::FloatVar& var = propertyConstraintSolver.getVariable(robot, constraint.getProperty());

        double value = constraint.getReferenceValue(robot);
        switch(constraint.getType())
        {
            case PropertyConstraint::EQUAL:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_EQ, value);
                break;
            case PropertyConstraint::LESS_EQUAL:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_LQ, value);
                break;
            case PropertyConstraint::LESS_THAN:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_LE, value);
                break;
            case PropertyConstraint::GREATER_EQUAL:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_GQ, value);
                break;
            case PropertyConstraint::GREATER_THAN:
                Gecode::rel(propertyConstraintSolver, var, Gecode::FRT_GR, value);
                break;
            default:
                break;
        }
    }

    propertyConstraintSolver.status();
    if(propertyConstraintSolver.failed())
    {
        return Fulfillment(false, constraints);
    } else {
        return Fulfillment(true, {});
    }
}

Gecode::FloatVar& PropertyConstraintSolver::getVariable(const owlapi::model::IRI& property)
{
    std::map<owlapi::model::IRI, Gecode::FloatVar>::iterator it = mValues.find(property);
    if(it == mValues.end())
    {
        mValues[property] = Gecode::FloatVar(*this, Gecode::Float::Limits::min, Gecode::Float::Limits::max);
        return mValues[property];
    }
    return it->second;
}

Gecode::FloatVar& PropertyConstraintSolver::getVariable(const facades::Robot& robot, const owlapi::model::IRI& property)
{
    std::map<owlapi::model::IRI, Gecode::FloatVar>::iterator it = mValues.find(property);
    if(it == mValues.end())
    {
        double value = robot.getPropertyValue(property);
        mValues[property] = Gecode::FloatVar(*this, value, value);

        return mValues[property];
    }
    return it->second;
}

} // end namespace templ
