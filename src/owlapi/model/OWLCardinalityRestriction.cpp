#include "OWLCardinalityRestriction.hpp"
#include "OWLObjectExactCardinality.hpp"
#include "OWLObjectMinCardinality.hpp"
#include "OWLObjectMaxCardinality.hpp"
#include <algorithm>
#include <sstream>
#include <boost/assign/list_of.hpp>

namespace owlapi {
namespace model {

std::map<OWLCardinalityRestriction::CardinalityRestrictionType, std::string> OWLCardinalityRestriction::CardinalityRestrictionTypeTxt = boost::assign::map_list_of
    (UNKNOWN, "UNKNOWN")
    (MIN, "MIN")
    (MAX, "MAX")
    (EXACT, "EXACT");

OWLCardinalityRestriction::Ptr OWLCardinalityRestriction::narrow() const
{
    if(getProperty()->isObjectPropertyExpression())
    {
        OWLObjectPropertyExpression::Ptr property = boost::dynamic_pointer_cast<OWLObjectPropertyExpression>( getProperty() );

        switch(getCardinalityRestrictionType())
        {
            case OWLCardinalityRestriction::MIN:
                return OWLCardinalityRestriction::Ptr( new OWLObjectMinCardinality(property, getCardinality(), getQualification()) ) ;
            case OWLCardinalityRestriction::MAX:
                return OWLCardinalityRestriction::Ptr(new OWLObjectMaxCardinality(property, getCardinality(), getQualification()) );
            case OWLCardinalityRestriction::EXACT:
                return OWLCardinalityRestriction::Ptr(new OWLObjectExactCardinality(property, getCardinality(), getQualification()) );
            default:
                throw std::runtime_error("OWLCardinalityRestriction::narrow: cardinality set to UNKNOWN cannot narrow");
        }
    } 
    throw std::runtime_error("OWLCardinalityRestriction::narrow: has not been implemented for dataproperties");
}


OWLCardinalityRestriction::OWLCardinalityRestriction()
    : OWLQualifiedRestriction( OWLPropertyExpression::Ptr(), OWLQualification("test"))
    , mCardinality(0)
    , mCardinalityRestrictionType(OWLCardinalityRestriction::UNKNOWN)
{}

OWLCardinalityRestriction::OWLCardinalityRestriction(OWLPropertyExpression::Ptr property, uint32_t cardinality, const OWLQualification& qualification, CardinalityRestrictionType restrictionType)
    : OWLQualifiedRestriction(property, qualification)
    , mCardinality(cardinality)
    , mCardinalityRestrictionType(restrictionType)
{}

std::string OWLCardinalityRestriction::toString() const
{
    std::stringstream ss;
    ss << "OWLCardinalityRestriction:" << std::endl;
    ss << "    property: " << getProperty()->toString() << std::endl;
    ss << "    cardinality: " << getCardinality() << std::endl;
    ss << "    qualification: " << getQualification().toString() << std::endl;
    ss << "    type: " << CardinalityRestrictionTypeTxt[getCardinalityRestrictionType()] << std::endl;
    return ss.str();
}

std::map<IRI, uint32_t> OWLCardinalityRestriction::convertToExactMapping(const std::vector<OWLCardinalityRestriction::Ptr>& restrictions)
{
    std::map<IRI, uint32_t> exactMapping;
    std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = restrictions.begin();
    for(; cit != restrictions.end(); ++cit)
    {
        OWLCardinalityRestriction::Ptr restriction = *cit;
        // Here we assume that 
        //  max 3 and max 4 -> exact 4
        //  min 3 and max 4 -> exact 4
        //  exact 1 and exact 2 -> exact 4
        uint32_t current = exactMapping[restriction->getQualification()];
        exactMapping[restriction->getQualification()] = std::max(current, restriction->getCardinality());
    }

    return exactMapping;
}

} // end namespace model
} // end namespace owlapi

