#include "SparqlInterface.hpp"
#include <sstream>
#include <boost/regex.hpp>
#include <base/Logging.hpp>

#include "Sparql.hpp"

namespace owl_om {
namespace db {
namespace query {

std::string Results::toString() const
{
    std::stringstream txt;
    std::vector<Row>::const_iterator rit = rows.begin();
    int count = 0;
    for(; rit != rows.end(); ++rit)
    {
        Row::const_iterator cit = rit->begin();

        txt << "--- #" << count++ << " ---" << std::endl;
        for(; cit != rit->end(); ++cit)
        {
            txt << "    " << cit->first << ": " << cit->second << std::endl;
        }
    }

    return txt.str();
}

ResultsIterator::ResultsIterator(const Results& results)
    : mResults(results)
    , mInitialized(false)
{
    mRowIterator = mResults.rows.begin();
}

bool ResultsIterator::next()
{
    if(mInitialized)
    {
        ++mRowIterator;
    } else {
        mInitialized = true;
    }

    return mRowIterator != mResults.rows.end();
}

Uri ResultsIterator::operator[](const std::string& name) const
{
    std::string variable = SparqlInterface::unboundVariable(name);

    Row::iterator cit =  mRowIterator->find(variable);
    if( mRowIterator->end() != cit)
    {
        return cit->second;
    }

    std::string bindings;
    cit = mRowIterator->begin();
    for(; cit != mRowIterator->end(); ++cit)
    {
        bindings += cit->first + ",";
    }
    std::string msg = "owl_om::db::query::ResultsIterator: unknown binding for '" + name + "' known are: " + bindings;
    throw std::runtime_error(msg);
}

Results SparqlInterface::findAll(const Uri& subject, const Uri& predicate, const Uri& object) const
{
    LOG_DEBUG_S << "Find all for: subject '" << subject << "', predicate: '" << predicate << "', object '" << object << "'";
    using namespace owl_om::db::rdf::sparql;
    Query query;
    bool doThrow = false;
    query.select(subject, doThrow) \
        .select(predicate, doThrow) \
        .select(object, doThrow) \
        .beginWhere() \
            .triple(subject,predicate,object) \
        .endWhere();

    std::string queryTxt = query.toString();
    LOG_DEBUG_S << "Sending query: " << queryTxt;
    return this->query(queryTxt, query.getBindings());
}

UnboundVariable SparqlInterface::unboundVariable(const std::string& name)
{
    if(name.empty())
    {
        throw std::invalid_argument("owl_om::Query::unboundVariable: given name is empty");
    }

    if(name.find("http://") != std::string::npos)
    {
        throw std::invalid_argument("owl_om::Query::unboundVariable: given URI: '" + name + "'");
    }

    std::string variable;
    if(name.data()[0] != '?')
    {
        variable = "?" + name;
    } else {
        variable = name;
    }
    return variable;
}

} // end namespace query
} // end namespace db
} // end namespace owl_om
