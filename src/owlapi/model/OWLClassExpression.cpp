#include "OWLClassExpression.hpp"
#include <boost/assign/list_of.hpp>
#include "OWLAxiomVisitor.hpp"

namespace owlapi {
namespace model {

std::map<OWLClassExpression::ClassExpressionType, std::string> OWLClassExpression::TypeTxt = boost::assign::map_list_of
    (DATA_ALL_VALUES_FROM, "DATA_ALL_VALUES_FROM")
    (DATA_HAS_VALUE, "DATA_HAS_VALUE")
    (DATA_EXACT_CARDINALITY, "DATA_EXACT_CARDINALITY")
    (DATA_MAX_CARDINALITY, "DATA_MAX_CARDINALITY")
    (DATA_MIN_CARDINALITY, "DATA_MIN_CARDINALITY")
    (DATA_SOME_VALUES_FROM, "DATA_SOME_VALUES_FROM")
    (OBJECT_ALL_VALUES_FROM, "OBJECT_ALL_VALUES_FROM")
    (OBJECT_COMPLEMENT_OF, "OBJECT_COMPLEMENT_OF")
    (OBJECT_HAS_SELF, "OBJECT_HAS_SELF")
    (OBJECT_HAS_VALUE, "OBJECT_HAS_VALUE")
    (OBJECT_INTERSECTION_OF, "OBJECT_INTERSECTION_OF")
    (OBJECT_EXACT_CARDINALITY, "OBJECT_EXACT_CARDINALITY")
    (OBJECT_MAX_CARDINALITY, "OBJECT_MAX_CARDINALITY")
    (OBJECT_MIN_CARDINALITY, "OBJECT_MIN_CARDINALITY")
    (OBJECT_ONE_OF, "OBJECT_ONE_OF")
    (OBJECT_SOME_VALUES_FROM, "OBJECT_SOME_VALUES_FROM")
    (OBJECT_UNION_OF, "OBJECT_UNION_OF")
    (OWL_CLASS, "OWL_CLASS");

} // end namespace model
} // end namespace owlapi
