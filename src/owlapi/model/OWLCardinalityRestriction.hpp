#ifndef OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP
#define OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP

#include <owl_om/owlapi/model/OWLQuantifiedRestriction.hpp>

namespace owlapi {
namespace model {

class OWLCardinalityRestriction : OWLQualifiedRestriction
{
    uint32_t mCardinality;

public:

    enum CardinalityType { MIN, MAX, EXACT };

    OWLCardinalityRestriction(const OWLPropertyExpression& property, uint32_t cardinality, const OWLQualification& qualification = IRI())
        : OWLQualifiedRestriction(property, qualification)
        , mCardinality(cardinality)
    {}

    uint32_t getCardinality() const { return mCardinality; }

    /**
     * Get the restriction type
     */
    virtual CardinalityType getCardinalityType() const = 0;

};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP
