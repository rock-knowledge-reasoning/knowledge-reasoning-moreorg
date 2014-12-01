#include "OWLQualifiedRestriction.hpp"

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

} // end namespace model
} // end namespace owlapi
