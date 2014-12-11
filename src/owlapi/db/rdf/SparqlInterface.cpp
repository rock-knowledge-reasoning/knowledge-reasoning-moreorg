#include "SparqlInterface.hpp"
#include <sstream>
#include <boost/regex.hpp>
#include <base/Logging.hpp>

#include "Sparql.hpp"

namespace owlapi {
namespace db {
namespace query {

Variable Any(const std::string& label)
{
    return Variable(label, false);
}

Variable Subject() { return Any("s"); }
Variable Object() { return Any("o"); }
Variable Predicate() { return Any("p"); }

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

owlapi::model::IRI ResultsIterator::operator[](const Variable& variable) const
{
    // Retrieve row that matches the requested variable
    Row::iterator cit =  mRowIterator->find(variable);
    if( mRowIterator->end() != cit)
    {
        return cit->second;
    }

    // For error handling
    std::string bindings;
    cit = mRowIterator->begin();
    for(; cit != mRowIterator->end(); ++cit)
    {
        Variable variable = cit->first;
        bindings += variable.getQueryName() + ",";
    }
    std::string msg = "owl_om::db::query::ResultsIterator: unknown binding for '" + variable.getQueryName() + "' known are: " + bindings;
    throw std::runtime_error(msg);
}

Results SparqlInterface::findAll(const Variable& subject, const Variable& predicate, const Variable& object) const
{
    LOG_DEBUG_S << "Find all for: subject '" << subject << "', predicate: '" << predicate << "', object '" << object << "'";
    using namespace owlapi::db::rdf::sparql;
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

} // end namespace query
} // end namespace db
} // end namespace owlapi
