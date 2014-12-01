#ifndef OWL_OM_DB_QUERY_SPARQL_INTERFACE_HPP
#define OWL_OM_DB_QUERY_SPARQL_INTERFACE_HPP

#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <owl_om/db/rdf/Variable.hpp>
#include <owl_om/Vocabulary.hpp>

namespace owl_om {
namespace db {
namespace query {

/**
 * Query variables
 */
extern Variable Subject();
extern Variable Object();
extern Variable Predicate();
extern Variable Any(const std::string& label);

typedef std::map<Variable, owlapi::model::IRI> Row;
struct Results
{
    std::vector<Row> rows;
    std::string toString() const;

    bool empty() { return rows.empty(); }
};

/**
 * \class ResultIterator
 */
class ResultsIterator
{
    Results mResults;
    std::vector<Row>::iterator mRowIterator;

    bool mInitialized;

public:
    ResultsIterator(const Results& results);

    bool next();

    owlapi::model::IRI operator[](const Variable& name) const;
};

/**
 * \class SparqlInterface
 * \brief Class for implementation for all database that allow sparql querying
 * \verbatim
 * SparqlInterface* si = ...
 * db::rdf::sparql::Query customQuery;
 * query.select(db::query::Subject())
 *      .beginWhere() \
 *      .triple(db::query::Subject(),vocabulary::RDF::type(),
 *              vocabulary::OWL::NamedIndividual()) \
 *      .endWhere();
 *
 * Results results = si->query(customQuery.toString(),
 * customQuery.getBindings());
 *
 * ResultIterator rit(results);
 * while(rit->next())
 * {
 *      IRI subject = rit[Subject()];
 * }
 * \endverbatim
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
    Results findAll(const Variable& subject, const Variable& predicate, const Variable& object) const;
};

} // end namespace query
} // end namespace db
} // end namespace owl_om
#endif // OWL_OM_DB_QUERY_SPARQL_INTERFACE_HPP
