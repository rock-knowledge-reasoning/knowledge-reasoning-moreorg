#ifndef OWL_API_MODEL_OWL_CLASS_EXPRESSION_HPP
#define OWL_API_MODEL_OWL_CLASS_EXPRESSION_HPP

#include <map>
#include <owl_om/owlapi/model/OWLObject.hpp>

namespace owlapi {
namespace model {

class OWLClassExpression : public OWLObject //, OWLPropertyRange, SWRLPredicate
{
public:
    enum ClassExpressionType { UNKNOWN
        , DATA_ALL_VALUES_FROM
        , DATA_EXACT_CARDINALITY
        , DATA_HAS_VALUE
        , DATA_MAX_CARDINALITY
        , DATA_MIN_CARDINALITY
        , DATA_SOME_VALUES_FROM
        , OBJECT_ALL_VALUES_FROM
        , OBJECT_COMPLEMENT_OF
        , OBJECT_HAS_SELF
        , OBJECT_HAS_VALUE
        , OBJECT_INTERSECTION_OF
        , OBJECT_EXACT_CARDINALITY
        , OBJECT_MAX_CARDINALITY
        , OBJECT_MIN_CARDINALITY
        , OBJECT_ONE_OF
        , OBJECT_SOME_VALUES_FROM
        , OBJECT_UNION_OF
        , OWL_CLASS
        , TYPE_RANGE_END
    };

    static std::map<ClassExpressionType, std::string> TypeTxt;

    /**
     * \brief Gets the class expression type for this class expression
     * \return the class expression type
     */
    virtual ClassExpressionType getClassExpressionType() { return UNKNOWN; }

    /**
     * \brief Determines whether or not this expression represents an anonymous class expression.
     * \return true if this is an anonymous class expression, false if this is a named class (OWLClass)
     */
    virtual bool isAnonymous() { throw std::runtime_error("OWLClassExpression::isAnonymous: not implemented"); }

    /**
     * \brief Determines if this class is a literal. A literal being either a named class or the negation of a named class (i.e. A or not(A)).
     * \return true if this is a literal, or false if this is not a literal.
     */
    virtual bool isClassExpressionLiteral() { throw std::runtime_error("OWLClassExpression::isClassExpressionLiteral: not implemented"); }
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_API_MODEL_OWL_CLASS_EXPRESSION_HPP
