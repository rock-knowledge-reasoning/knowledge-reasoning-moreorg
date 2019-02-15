#ifndef ORGANIZATION_MODEL_PROPERTY_CONSTRAINT_SOLVER_HPP
#define ORGANIZATION_MODEL_PROPERTY_CONSTRAINT_SOLVER_HPP

#include <gecode/search.hh>
#include <gecode/float.hh>
#include "PropertyConstraint.hpp"
#include "ValueBound.hpp"

namespace organization_model {
namespace facades {
    class Robot;
}

class Fulfillment
{
public:
    Fulfillment(bool isMet, const PropertyConstraint::Set& conflicts)
        : mFulfilled(isMet)
        , mConflictingConstraints(conflicts)
    {}

    bool isMet() const { return mFulfilled; }
    const PropertyConstraint::Set& conflicts() const { return mConflictingConstraints; }
private:
    bool mFulfilled;
    const PropertyConstraint::Set& mConflictingConstraints;

};

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
     * Merge a list of constraints and returns the corresponding allowed value bound
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

    /**
     * Test a list of constraints
     * \param robot Robot reference to dynamically resolve property values for * composite systems
     * \throws std::invalid_argument when constraints cannot be fulfilled
     */
    static Fulfillment fulfills(const facades::Robot& robot, const PropertyConstraint::List& constraints);

    /**
     * Check if a robot fulfills a list of constraints
     * \param robot Robot reference to dynamically resolve property values for * composite systems
     * \return True if the robot fulfills the constraints, false if not
     * \throws std::invalid_argument when constraints cannot be fulfilled
     */
    static Fulfillment fulfills(const facades::Robot& robot, const PropertyConstraint::Set& constraints);

protected:
    std::map<owlapi::model::IRI, Gecode::FloatVar> mValues;

    /**
     * Get the variable that maps to a particular property
     * \return variable
     */
    Gecode::FloatVar& getVariable( const owlapi::model::IRI& property);

    /**
     * Get the float variable bound to a particular robot's property value
     */
    Gecode::FloatVar& getVariable(const facades::Robot& robot, const owlapi::model::IRI& property);

};

} // end namespace templ
#endif // ORGANIZATION_MODEL_PROPERTY_CONSTRAINT_SOLVER_HPP
