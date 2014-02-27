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

typedef std::map<std::string, std::string> Row;
struct Results
{
    std::vector<Row> rows;

    std::string toString() const;
};

class ResultsIterator
{
public:
    ResultsIterator(const Results& results)
        : mResults(results)
    {
        mRowIterator = mResults.rows.begin();
    }

    bool next() 
    {
        mCurrentRowPtr = const_cast<Row*>( &(*mRowIterator));
        return mRowIterator++ != mResults.rows.end();
    }

    Uri operator[](const std::string& name) const
    {
        Row::const_iterator cit =  mCurrentRowPtr->find(name);
        if( mCurrentRowPtr->end() != cit)
        {
            return cit->second;
        }
        std::string msg = "owl_om::db::query::ResultsIterator: unknown binding '" + name + "'";
        throw std::runtime_error(msg);
    }

private:
    Results mResults;
    Row* mCurrentRowPtr;
    std::vector<Row>::const_iterator mRowIterator;
};

class SparqlInterface
{
public:
    Results select_where(const Uri& subject, const Uri& predicate, const Uri& object, const Bindings& binding);

protected:
    virtual Results query(const std::string& query, const Bindings& bindings) { throw std::runtime_error("owl_om::db::query::SparqlInterface not implemented"); }
};

class Utils
{
public:
    static Uri identifyPlaceholder(const std::string& uri, Bindings* bindings);
};

} // end namespace query
} // end namespace db
} // end namespace owl_om
#endif // OWL_OM_DB_QUERY_SPARQL_INTERFACE_HPP
