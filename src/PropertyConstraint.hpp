#ifndef ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP
#define ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP

#include <vector>
#include <set>
#include <map>
#include <owlapi/model/IRI.hpp>

namespace organization_model {
namespace facades {
    class Robot;
}

/**
 * A property constraint represents a triple P RT D, where
 * P represent the property, RT the relation/constraint type and D the data
 * value
 * \todo Currently PropertyConstraints only represent numeric constraints,
 * though the general hasValue,allValuesFrom restrictions from OWL could/should
 * be considered here
 */
class PropertyConstraint
{
public:
    typedef std::vector<PropertyConstraint> List;
    typedef std::set<PropertyConstraint> Set;
    typedef std::map<owlapi::model::IRI, PropertyConstraint::Set> Clusters;

    enum ConstraintType {
        /// Unknown constraint type
        UNKNOWN,
        /// ==
        EQUAL,
        // <
        LESS_THAN,
        // <=
        LESS_EQUAL,
        // >=
        GREATER_EQUAL,
        // >
        GREATER_THEN,
        // end marker for iterators
        CONSTRAINT_TYPE_END
    };

    static std::map<ConstraintType, std::string> TypeTxt;

    /**
     * Default constructor to allow use in map
     */
    PropertyConstraint();

    /**
     * Standard constructor for a property constraint
     * \param dataProperty The identifier for the property
     * \param c constraint/relation type
     * \param value Constraining value
     */
    PropertyConstraint(const owlapi::model::IRI& dataProperty, ConstraintType c, double value);

    /**
     * Standard constructor for a property constraint
     * \param dataProperty The identifier for the property
     * \param c constraint/relation type
     * \param rvalDataProperty Reference to constraining data property
     */
    PropertyConstraint(const owlapi::model::IRI& dataProperty, ConstraintType c,
            const owlapi::model::IRI& rvalDataProperty);

    /**
     * Get the constraint type
     * \return constraint type
     */
    ConstraintType getType() const { return mType; }

    /**
     * Get the (data) property
     * \return property
     */
    const owlapi::model::IRI& getProperty() const { return mDataProperty; }

    /**
     * Get the value
     * \return constraining value
     */
    double getValue() const { return mValue; }

    /**
     * Dynamically retrieve a valueProperty with respect to a robot (maybe composite system);
     */
    double getValue(const facades::Robot& robot) const;

    /**
     * Stringify this object
     * \return string representing this PropertyConstraint
     */
    std::string toString() const;

    /**
     * Cluster the set of constraints, based on the referred property
     * \return Set of Clusters
     */
    static Clusters getClusters(const PropertyConstraint::Set& constraints);

    /**
     * Check equality of two PropertyConstraint object
     * \param other Other PropertyConstraint to compare against
     * \return True, if they are equal false otherwise
     */
    bool operator==(const PropertyConstraint& other) const;

    /**
     * Add less operator for using property constraint in sets
     * \param other Other PropertyConstraint to compare against
     * \return True, if current object is less compared to other
     */
    bool operator<(const PropertyConstraint& other) const;

private:
    ConstraintType mType;
    owlapi::model::IRI mDataProperty;
    double mValue;
    /// If the property constraint refers to a property that has
    /// to be dynamically extracted
    /// mass < transportCapacity
    owlapi::model::IRI mRValProperty;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_PROPERTY_CONSTRAINTS_HPP
