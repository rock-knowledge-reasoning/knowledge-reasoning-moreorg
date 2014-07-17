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
        prefixes += "PREFIX " + cit->first + ": " + cit->second.toQuotedString() + "\n";
    }
    return prefixes;
}

std::string Query::selectTxt() const
{
    std::string select;
    db::query::VariableList::const_iterator cit = mSelect.begin();
    select += "SELECT";
    for(; cit != mSelect.end(); ++cit)
    {
        if(cit->isGrounded())
        {
            LOG_DEBUG_S << "Skipping ground variable '" << cit->getQueryName() + "' for SELECT";
            continue;
        }
        select += " " + cit->toString();
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

//void Query::addUnboundVariable(const db::query::Variable& variable)
//{
//    if(!variable.isGrounded())
//    {
//        db::query::VariableList::const_iterator cit = std::find(mUnboundVariables.begin(), mUnboundVariables.end(), variable);
//        if(cit == mUnboundVariables.end())
//        {
//            mUnboundVariables.push_back(variable);
//        } else {
//            LOG_DEBUG_S << "Unbound variable '" << variable << "' already registered";
//        }
//    } else {
//        throw std::invalid_argument("owl_om::db::query::Query::addUnboundVariable: trying to add grounded variable");
//    }
//}

std::string Query::getOrCreatePrefix(const owlapi::model::IRI& iri)
{
    PrefixMap::const_iterator cit = mPrefixes.begin();
    for(; cit != mPrefixes.end(); ++cit)
    {
        if(cit->second == iri)
        {
            // found existing prefix definition for given IRI
            return cit->first;
        }
    }

    // autogenerate a prefix
    std::stringstream ss;
    ss << "auto_prefix_" << mPrefixes.size();
    std::string prefix = ss.str();
    mPrefixes[prefix] = iri;
    return prefix;
}

//void Query::prepare(const db::query::Variable& variable)
//{
//    if(!variable.isGrounded())
//    {
//        addUnboundVariable(variable);
//    }
//
//            
//    //if(uriOrVariable.empty())
//    //{
//    //    throw std::invalid_argument("owl_om::db::rdf::Sparql::prepare given uri or variable is empty");
//    //}
//    //// need to extract prefixes from a single uri, list or uri or chained uris
//    //// if the argument is not an uri, check if the variable is already known
//    //
//    //size_t position = uriOrVariable.find("http://");
//    //if(position == std::string::npos)
//    //{
//    //    // must be a variable or prefixed url
//    //    size_t positionColon = uriOrVariable.find_first_of(":");
//    //    if(positionColon == std::string::npos)
//    //    {
//    //        LOG_DEBUG_S << "Identified '" << uriOrVariable << "' as unbound variable";
//    //        std::string variable = query::Variable(uriOrVariable, true)
//    //        addUnboundVariable(variable);
//    //        return variable;
//    //    } else {
//    //        // found prefix so check if this is valid
//    //        std::string prefix = uriOrVariable.substr(0,positionColon);
//    //        if( !mPrefixes.count(prefix) )
//    //        {
//    //            throw std::invalid_argument("Invalid use of undefined prefix '" + prefix  + "'");
//    //        } else {
//    //            // properly prefixed, so can be used without modification in query
//    //            return uriOrVariable;
//    //        }
//    //    }
//    //} else {
//    //    using namespace owlapi::model;
//    //    IRI iri = IRI::create(uriOrVariable);
//    //    std::string prefix = getOrCreatePrefix( IRI(iri.getPrefix(),""));
//
//    //    return prefix + ":" + iri.getRemainder();
//    //}
//
//    //throw std::runtime_error("owl_om::db::sparql::Query::prepare Failed to prepare variable '" + uriOrVariable + "'");
//}

Query::Type Query::getType() const
{
    if(!mSelect.empty())
    {
        return Query::SELECT;
    } else {
        return Query::UNKNOWN;
    }
}

Query& Query::prefix(const std::string& prefix, const owlapi::model::IRI& iri)
{
    if(!mPrefixes.count(prefix))
    {
        mPrefixes[prefix] = iri;
    }

    return *this;
}

Query& Query::select(const db::query::Variable& variable, bool do_throw)
{
    //db::query::Variable unboundVariable;
    //try {
    //    unboundVariable = db::query::SparqlInterface::unboundVariable(variable.toString());
    //} catch(const std::invalid_argument& e)
    //{
    //    // Not an unbound variable 
    //    if(do_throw)
    //    {
    //        throw std::invalid_argument("owl_om::Query::select: '" + variable.toString() + "' is not an unbound variable");
    //    }
    //    return *this;
    //}
    if(variable.isGrounded() && do_throw)
    {
        throw std::invalid_argument("owl_om::Query::select: '" + variable.toString() + "' is a grounded variable");
    }

    db::query::VariableList::const_iterator cit = std::find(mSelect.begin(), mSelect.end(), variable);
    if(cit != mSelect.end())
    {
        if(do_throw)
        {
            throw std::invalid_argument("owl_om::Query::select: '" + variable.getQueryName() + "' has already been added to list of 'select'");
        } else {
            return *this;
        }
    }
    mSelect.push_back(variable);

    return *this;
}

db::query::Bindings Query::getBindings() const
{
    db::query::VariableList::const_iterator cit = mSelect.begin();
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

WhereClause& WhereClause::triple(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object)
{
    mExpressionList.push_back(subject.getQueryName() + " " + predicate.getQueryName() + " " + object.getQueryName() + " .");
    return *this;
}

WhereClause& WhereClause::filter_regex(const db::query::Variable& variable, const std::string& matchExpression, filter::Option option)
{
    throw std::runtime_error("filter_regex: not implemented yet");
}

WhereClause& WhereClause::filter_expression(const db::query::Variable& variable, const std::string& op, const std::string& expression)
{
    throw std::runtime_error("filter_regex: not implemented yet");
}

WhereClause& WhereClause::minus(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object)
{
    mMinusExpressionList.push_back(subject.getQueryName() + " " + predicate.getQueryName() + " " + object.getQueryName());
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
