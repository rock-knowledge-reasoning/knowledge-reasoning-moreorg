#ifndef OWL_OM_DB_QUERY_UNBOUND_VARIABLE_HPP
#define OWL_OM_DB_QUERY_UNBOUND_VARIABLE_HPP

#include <owl_om/OWLApi.hpp>
#include <vector>

namespace owl_om {
namespace db {
namespace query {

/**
 * Variable represent a query object. It can be either an
 * ungrounded variable such as '?s' or a prefix or fully qualified IRI
 */
class Variable : public owlapi::model::IRI
{
public:
    /**
     * Default Variable constructor
     */
    Variable();

    /**
     * Constructor to create variable from IRI
     */
    Variable(const IRI& iri);

    /**
     * Constructor to create ground or ungrounded variables
     */
    Variable(const std::string& name, bool grounded);

    /**
     * Get name of this variable, i.e. for an ungrounded variable
     * it removes the leading question mark
     */
    std::string getName() const;

    /**
     * Get query name of this variable, ungrounded variables
     * will be presented with leading question marks, 
     * and URI will be quoted, e.g., <http://www.w3.org/>
     * \return Query name of this variable
     */
    std::string getQueryName() const;

    /**
     * Test if variable is grounded or just represents a placeholder
     */
    bool isGrounded() const;
};

typedef std::vector<Variable> VariableList;
typedef VariableList Bindings;


} // end namespace query
} // end namespace db
} // end namespace owl_om
#endif // OWL_OM_DB_QUERY_UNBOUND_VARIABLE_HPP
