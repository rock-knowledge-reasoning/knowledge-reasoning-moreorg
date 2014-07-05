#ifndef OWL_OM_DB_QUERY_UNBOUND_VARIABLE_HPP
#define OWL_OM_DB_QUERY_UNBOUND_VARIABLE_HPP

#include <owl_om/OWLApi.hpp>
#include <vector>

namespace owl_om {
namespace db {
namespace query {

class Variable : public owlapi::model::IRI
{
public:
    Variable();
    Variable(const IRI& iri);
    Variable(const std::string& name, bool grounded);

    std::string getName() const;
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
