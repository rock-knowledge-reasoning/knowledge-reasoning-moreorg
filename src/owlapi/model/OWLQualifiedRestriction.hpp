#ifndef OWL_API_MODEL_QUALIFIED_RESTRICTION_HPP
#define OWL_API_MODEL_QUALIFIED_RESTRICTION_HPP

namespace owlapi {
namespace model {

typedef IRI OWLQualification;

class OWLQualifiedRestriction : public OWLRestriction
{

public:
    OWLQualifiedRestriction(const OWLPropertyExpression& property, const OWLQualification& qualification)
        : OWLRestriction(property)
    {
        if(qualification == IRI())
        {
            if(isDataRestriction())
            {
                mQualification = vocabulary::RDFS::Literal();
            } else {
                mQualification = vocabulary::OWL::Thing();
            }
        } else {
            mQualification = qualification;
        }
    }

    OWLQualificiation getQualification() const { return mQualification; }

    /**
     * \details Determines if this restriction is qualified. Qualified cardinality restrictions are defined to be cardinality restrictions that have fillers which aren't TOP (owl:Thing or rdfs:Literal). An object restriction is unqualified if it has a filler that is owl:Thing. A data restriction is unqualified if it has a filler which is the top data type (rdfs:Literal).
     */
    bool isQualified() const
    {
        if(mQualification == vocabulary::OWL::Thing() && mQualification == vocabulary::RDFS::Literal())
        {
            return false;
        }
        return true;
    }

};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_QUALIFIED_RESTRICTION_HPP
