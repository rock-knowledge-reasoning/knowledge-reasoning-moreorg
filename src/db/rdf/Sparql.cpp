#include "Sparql.hpp"
#include <boost/assign/list_of.hpp>
#include <boost/regex.hpp>
#include <base/Logging.hpp>
#include <sstream>

#include <owl_om/Vocabulary.hpp>

namespace owl_om {
namespace db {
namespace rdf {
namespace sparql {

std::map<Query::Type, std::string> Query::TypeTxt = boost::assign::map_list_of
    (Query::UNKNOWN, "UNKNOWN")
    (Query::SELECT, "SELECT")
    (Query::CONSTRUCT, "CONSTRUCT")
    (Query::ASK, "ASK")
    (Query::DESCRIBE, "DESCRIBE");

Query::Query()
    : mWhere(this)
{
}

std::string Query::prefixTxt() const
{
    std::string prefixes;
    PrefixMap::const_iterator cit = mPrefixes.begin();
    for(; cit != mPrefixes.end(); ++cit)
    {
        prefixes += "PREFIX " + cit->first + ": <" + cit->second + ">\n"; 
    }
    return prefixes;
}

std::string Query::selectTxt() const
{
    std::string select;
    db::query::UnboundVariableList::const_iterator cit = mSelect.begin();
    select += "SELECT";
    for(; cit != mSelect.end(); ++cit)
    {
        select += " " + *cit;
    }
    select += "\n";
    return select;
}

std::string Query::whereTxt() const
{
    std::string where;
    where += "WHERE {\n";
    where += mWhere.toString();
    where += "}";

    return where;
}

void Query::addUnboundVariable(const db::query::UnboundVariable& variable)
{
    db::query::UnboundVariableList::const_iterator cit = std::find(mUnboundVariables.begin(), mUnboundVariables.end(), variable);
    if(cit == mUnboundVariables.end())
    {
        mUnboundVariables.push_back(variable);
    } else {
        LOG_DEBUG_S << "Unbound variable '" << variable << "' already registered";
    }
}

std::string Query::getOrCreatePrefix(const Uri& uri)
{
    PrefixMap::const_iterator cit = mPrefixes.begin();
    for(; cit != mPrefixes.end(); ++cit)
    {
        if(cit->second == uri)
        {
            // found existing prefix definition
            return cit->first;
        }
    }

    // autogenerate a prefix
    std::stringstream ss;
    ss << "auto_prefix_" << mPrefixes.size();
    std::string prefix = ss.str();
    mPrefixes[prefix] = uri;
    return prefix;
}

std::string Query::prepare(const std::string& uriOrVariable)
{
    if(uriOrVariable.empty())
    {
        throw std::invalid_argument("owl_om::db::rdf::Sparql::prepare given uri or variable is empty");
    }
    // need to extract prefixes from a single uri, list or uri or chained uris
    // if the argument is not an uri, check if the variable is already known
    
    size_t position = uriOrVariable.find("http://");
    if(position == std::string::npos)
    {
        // must be a variable or prefixed url
        size_t positionColon = uriOrVariable.find_first_of(":");
        if(positionColon == std::string::npos)
        {
            LOG_DEBUG_S << "Identified '" << uriOrVariable << "' as unbound variable";
            std::string variable = query::SparqlInterface::unboundVariable(uriOrVariable);
            addUnboundVariable(variable);
            return variable;
        } else {
            // found prefix so check if this is valid
            std::string prefix = uriOrVariable.substr(0,positionColon);
            if( !mPrefixes.count(prefix) )
            {
                throw std::invalid_argument("Invalid use of undefined prefix '" + prefix  + "'");
            } else {
                // properly prefixed, so can be used without modification in query
                return uriOrVariable;
            }
        }
    } else {
        vocabulary::SplitUri splitUri = vocabulary::Utils::extractBaseUri(uriOrVariable);
        std::string prefix = getOrCreatePrefix(splitUri.baseUri);

        return prefix + ":" + splitUri.name;
    }

    throw std::runtime_error("owl_om::db::sparql::Query::prepare Failed to prepare variable '" + uriOrVariable + "'");
}

Query::Type Query::getType() const
{
    if(!mSelect.empty())
    {
        return Query::SELECT;
    } else {
        return Query::UNKNOWN;
    }
}

Query& Query::prefix(const std::string& prefix, const Uri& uri)
{
    if(!mPrefixes.count(prefix))
    {
        mPrefixes[prefix] = uri;
    }

    return *this;
}

Query& Query::select(const db::query::UnboundVariable& variable, bool do_throw)
{
    db::query::UnboundVariable unboundVariable;
    try {
        unboundVariable = db::query::SparqlInterface::unboundVariable(variable);
    } catch(const std::invalid_argument& e)
    {
        // Not an unbound variable 
        if(do_throw)
        {
            throw std::invalid_argument("owl_om::Query::select: '" + variable + "' is not an unbound variable");
        }
        return *this;
    }

    db::query::UnboundVariableList::const_iterator cit = std::find(mSelect.begin(), mSelect.end(), unboundVariable);
    if(cit != mSelect.end())
    {
        if(do_throw)
        {
            throw std::invalid_argument("owl_om::Query::select: '" + variable + "' has already been added to list of 'select'");
        } else {
            return *this;
        }
    }
    mSelect.push_back(variable);

    return *this;
}

db::query::Bindings Query::getBindings() const
{
    db::query::UnboundVariableList::const_iterator cit = mSelect.begin();
    db::query::Bindings bindings;
    for(; cit != mSelect.end(); ++cit)
    {
        bindings.push_back(*cit);
    }
    return bindings;
}

std::string Query::toString() const
{
    std::string query;
    query += prefixTxt() + " ";

    switch(getType())
    {
        case Query::SELECT:
            query += selectTxt() + " ";
            break;
        default:
            throw std::invalid_argument("owl_om::Query::toString: query type '" + TypeTxt[getType()] + "' is not supported");
    }
    query += whereTxt();

    return query;
}

WhereClause::WhereClause(Query* query)
    : mQuery(query)
{
    if(!mQuery)
    {
        throw std::runtime_error("owl_om::WhereClause(): associated query object is NULL");
    }
}

WhereClause& WhereClause::triple(const std::string& subject, const std::string& predicate, const std::string& object)
{
    std::string s = mQuery->prepare(subject);
    std::string p = mQuery->prepare(predicate);
    std::string o = mQuery->prepare(object);

    mExpressionList.push_back(s + " " + p + " " + o + " .");
    return *this;
}

WhereClause& WhereClause::filter_regex(const db::query::UnboundVariable& variable, const std::string& matchExpression, filter::Option option)
{
    throw std::runtime_error("filter_regex: not implemented yet");
}

WhereClause& WhereClause::filter_expression(const db::query::UnboundVariable& variable, const std::string& op, const std::string& expression)
{
    throw std::runtime_error("filter_regex: not implemented yet");
}

WhereClause& WhereClause::minus(const std::string& subject, const std::string& predicate, const std::string& object)
{
    std::string s = mQuery->prepare(subject);
    std::string p = mQuery->prepare(predicate);
    std::string o = mQuery->prepare(object);

    mMinusExpressionList.push_back(s + " " + p + " " + o);
    return *this;
}

Query& WhereClause::endWhere()
{
    if(!mQuery)
    {
        throw std::runtime_error("owl_om::WhereClause::endwhere: associated query object is NULL");
    }
    return *mQuery;
}

std::string WhereClause::toString() const
{
    std::string where;
    std::vector<std::string>::const_iterator cit = mExpressionList.begin();
    for(; cit != mExpressionList.end(); ++cit)
    {
        where += *cit + "\n";
    }
    if(!mMinusExpressionList.empty())
    {
        where += "MINUS {\n";
        std::vector<std::string>::const_iterator mit = mMinusExpressionList.begin();
        for(; mit != mMinusExpressionList.end(); ++mit)
        {
            where += *mit + " .\n";
        }
        where += "}\n";
    }
    return where;
}

} // end namespace sparql
} // end namespace rdf
} // end namespace db
} // end namespace owl_om
