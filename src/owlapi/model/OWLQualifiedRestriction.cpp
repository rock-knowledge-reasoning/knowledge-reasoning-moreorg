#include "OWLQualifiedRestriction.hpp"
#include <sstream>

namespace owlapi {
namespace model {

OWLQualifiedRestriction::OWLQualifiedRestriction(OWLPropertyExpression::Ptr property, const OWLQualification& qualification)
    : OWLRestriction(property)
{
    if(qualification == IRI())
    {
        if(isDataRestriction())
        {
            mQualification = owl_om::vocabulary::RDFS::Literal();
        } else {
            mQualification = owl_om::vocabulary::OWL::Thing();
        }
    } else {
        mQualification = qualification;
    }

    mQualified = !( mQualification == owl_om::vocabulary::OWL::Thing() || mQualification == owl_om::vocabulary::RDFS::Literal() );
}

std::string OWLQualifiedRestriction::toString() const
{
    std::stringstream ss;
    ss << "OWLQualifiedRestriction:" << std::endl;
    ss << "    property: " << getProperty()->toString() << std::endl;
    ss << "    qualification: " << getQualification().toString() << std::endl;
    return ss.str();
}

} // end namespace model
} // end namespace owlapi
