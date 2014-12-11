#ifndef OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP
#define OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP

#include <vector>
#include <stdint.h>
#include <owl_om/owlapi/model/OWLQualifiedRestriction.hpp>

namespace owl_om {
    class Ontology;
}

namespace owlapi {
namespace model {

/**
 * \class OWLCardinalityRestriction
 */
class OWLCardinalityRestriction : public OWLQualifiedRestriction
{
    friend class owl_om::Ontology;

public:
    typedef boost::shared_ptr<OWLCardinalityRestriction> Ptr;

    enum CardinalityRestrictionType { UNKNOWN, MIN, MAX, EXACT };

    static std::map<CardinalityRestrictionType, std::string> CardinalityRestrictionTypeTxt;

    /**
     * Default constructor to allow usage of this class in a map
     */
    OWLCardinalityRestriction();

    OWLCardinalityRestriction(OWLPropertyExpression::Ptr property, uint32_t cardinality, const OWLQualification& qualification, CardinalityRestrictionType restrictionType);

    uint32_t getCardinality() const { return mCardinality; }

    /**
     * Get the restriction type
     */
    virtual CardinalityRestrictionType getCardinalityRestrictionType() const { return mCardinalityRestrictionType; }

    virtual std::string toString() const;

protected:
    /**
     * Set cardinality -- e.g. to allow incremental construction of restrictions
     */
    void setCardinality(uint32_t cardinality) { mCardinality = cardinality; }

    void setCardinalityRestrictionType(OWLCardinalityRestriction::CardinalityRestrictionType type) { mCardinalityRestrictionType = type; }

    /**
     * Depending on the property type, narrow the internally used class to
     * either OWLObject.. or OWLData
     */
    OWLCardinalityRestriction::Ptr narrow() const;

private:
    uint32_t mCardinality;
    CardinalityRestrictionType mCardinalityRestrictionType;
};

} // end namespace model
} // end namespace owlapi

#endif // OWL_API_MODEL_CARDINALITY_RESTRICTION_HPP
