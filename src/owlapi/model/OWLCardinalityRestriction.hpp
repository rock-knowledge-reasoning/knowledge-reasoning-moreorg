#ifndef OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP
#define OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP

#include <stdint.h>
#include <owl_om/owlapi/model/OWLQualifiedRestriction.hpp>

namespace owlapi {
namespace model {

class OWLCardinalityRestriction : public OWLQualifiedRestriction
{
public:

    enum CardinalityRestrictionType { MIN, MAX, EXACT };

    OWLCardinalityRestriction(OWLPropertyExpression::Ptr property, uint32_t cardinality, const OWLQualification& qualification, CardinalityRestrictionType restrictionType)
        : OWLQualifiedRestriction(property, qualification)
        , mCardinality(cardinality)
        , mCardinalityRestrictionType(restrictionType)
    {}

    uint32_t getCardinality() const { return mCardinality; }

    /**
     * Get the restriction type
     */
    virtual CardinalityRestrictionType getCardinalityRestrictionType() const { return mCardinalityRestrictionType; }

private:
    uint32_t mCardinality;
    CardinalityRestrictionType mCardinalityRestrictionType;
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP
