#ifndef OWLAPI_DB_SPARQL_HPP
#define OWLAPI_DB_SPARQL_HPP

#include <vector>
#include <map>
#include <stdexcept>
#include <owl_om/owlapi/db/rdf/SparqlInterface.hpp>

namespace owlapi {
namespace db {
namespace rdf {
namespace sparql {

class Query;

namespace filter {
    enum Option { UNKNOWN_OPTION = 0x00, IGNORE_CASE = 0x01 };
}

class WhereClause
{
    Query* mQuery;
    std::vector<std::string> mExpressionList;
    std::vector<std::string> mMinusExpressionList;

public:
    WhereClause(Query* query);
    /**
     * Add a triple statement
     */
    WhereClause& triple(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object);
    WhereClause& filter_regex(const db::query::Variable& variable, const std::string& matchExpression, filter::Option option = filter::UNKNOWN_OPTION);
    WhereClause& filter_expression(const db::query::Variable& variable, const std::string& op, const std::string& expression);
    WhereClause& minus(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object);

    /**
     * Return the associated mQuery
     */
    Query& endWhere();

    std::string toString() const;
};

/**
 * \class Query
 * \brief Query of that allow to create SPARQL queries programatically
 *
 */
class Query
{
    friend class WhereClause;

    typedef std::map<std::string, owlapi::model::IRI> PrefixMap;

    PrefixMap mPrefixes;
    db::query::VariableList mSelect;
    WhereClause mWhere;

    std::string prefixTxt() const;
    std::string selectTxt() const;
    std::string whereTxt() const;

    /**
     * Add an unbound variable that will be used in this query
     */
    //void addUnboundVariable(const db::query::Variable& variable);

    std::string getOrCreatePrefix(const owlapi::model::IRI& iri);

protected:
    /**
     * Prepare an given uri, by extracting the prefix or check
     * if the given variable is valid
     */
    //std::string prepare(const std::string& uriOrVariable);

public:

    Query();

    enum Type { UNKNOWN, SELECT, CONSTRUCT, ASK, DESCRIBE };

    Type getType() const;

    static std::map<Type, std::string> TypeTxt;

    /**
     * Add a prefix
     * \param prefix
     * \param uri Uri this prefix maps to
     */
    Query& prefix(const std::string& prefix, const owlapi::model::IRI& iri);

    /**
     * Add a variable to the select part of this query
     */
    Query& select(const db::query::Variable& variable, bool do_throw = true);

    /**
     * Start definition of the where part of the query
     */
    WhereClause& beginWhere() { return mWhere; }

    /**
     * Retrieve bindings of this query, i.e. extracted from the select part
     */
    db::query::Bindings getBindings() const;

    /**
     * Convert query to string
     */
    std::string toString() const;
};


} // end namespace sparql
} // end namespace rdf
} // end namespace db
} // end namespace owlapi
#endif // OWLAPI_DB_SPARQL_HPP
