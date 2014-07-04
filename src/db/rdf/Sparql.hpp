#ifndef OWL_OM_DB_SPARQL_HPP
#define OWL_OM_DB_SPARQL_HPP

#include <vector>
#include <map>
#include <stdexcept>
#include <owl_om/Uri.hpp>
#include <owl_om/db/rdf/SparqlInterface.hpp>

namespace owl_om {
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
    WhereClause& triple(const std::string& subject, const std::string& predicate, const std::string& object);
    WhereClause& filter_regex(const db::query::UnboundVariable& variable, const std::string& matchExpression, filter::Option option = filter::UNKNOWN_OPTION);
    WhereClause& filter_expression(const db::query::UnboundVariable& variable, const std::string& op, const std::string& expression);
    WhereClause& minus(const std::string& subject, const std::string& predicate, const std::string& object);

    /**
     * Return the associated mQuery
     */
    Query& endWhere();

    std::string toString() const;
};

class Query 
{
    friend class WhereClause;

    typedef std::map<std::string, Uri> PrefixMap;

    db::query::UnboundVariableList mUnboundVariables;

    PrefixMap mPrefixes;
    db::query::UnboundVariableList mSelect;
    WhereClause mWhere;

    std::string prefixTxt() const;
    std::string selectTxt() const;
    std::string whereTxt() const;

    /**
     * Add an unbound variable that will be used in this query
     */
    void addUnboundVariable(const db::query::UnboundVariable& variable);

    std::string getOrCreatePrefix(const Uri& uri);

protected:
    /**
     * Prepare an given uri, by extracting the prefix or check
     * if the given variable is valid
     */
    std::string prepare(const std::string& uriOrVariable);

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
    Query& prefix(const std::string& prefix, const Uri& uri);
    Query& select(const db::query::UnboundVariable& variable, bool do_throw = true);
    WhereClause& beginWhere() { return mWhere; }

    db::query::Bindings getBindings() const;

    std::string toString() const;
};


} // end namespace sparql
} // end namespace rdf
} // end namespace db
} // end namespace owl_om
#endif // OWL_OM_DB_SPARQL_HPP
