#ifndef OWL_OM_DB_QUERY_SPARQL_INTERFACE_HPP
#define OWL_OM_DB_QUERY_SPARQL_INTERFACE_HPP

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <owl_om/Vocabulary.hpp>

namespace owl_om {
namespace db {
namespace query {

typedef std::vector<std::string> Bindings;
typedef std::string UnboundVariable;
typedef std::vector<UnboundVariable> UnboundVariableList;

typedef std::map<std::string, std::string> Row;
struct Results
{
    std::vector<Row> rows;
    std::string toString() const;
};

class ResultsIterator
{
    Results mResults;
    std::vector<Row>::iterator mRowIterator;

    bool mInitialized;

public:
    ResultsIterator(const Results& results);

    bool next();

    Uri operator[](const std::string& name) const;
};

/**
 * \class SparqlInterface
 * \brief Class for implementation for all database that allow sparql querying
 */
class SparqlInterface
{
public:
    virtual ~SparqlInterface() {}

    /**
     * Send a sparql query to the underlying database using the given bindings
     * \param query The SPARQL conform query
     * \param bindings The set of bindings used
     */
    virtual Results query(const std::string& query, const Bindings& bindings) const { throw std::runtime_error("owl_om::db::query::SparqlInterface not implemented"); }

    /**
     * Retrieve results when matching the given triple definition
     * \return Results List of Rows
     */
    Results findAll(const Uri& subject, const Uri& predicate, const Uri& object) const;

    /**
     * Create an unbound variable, i.e. with preceding '?'
     */
    static UnboundVariable unboundVariable(const std::string& name);
};

} // end namespace query
} // end namespace db
} // end namespace owl_om
#endif // OWL_OM_DB_QUERY_SPARQL_INTERFACE_HPP
