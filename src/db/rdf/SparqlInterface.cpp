#include "SparqlInterface.hpp"
#include <boost/regex.hpp>
#include <sstream>
#include <base/Logging.hpp>

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
            txt << "    " << cit->second << std::endl;
        }
    }

    return txt.str();
}

Results SparqlInterface::select_where(const Uri& subject, const Uri& predicate, const Uri& object, const Bindings& bindings)
{
    std::string select = "select ";
    std::string prefix;
    std::string where;

    std::map<std::string,std::string> prefixOpenlist;
    // using mini prefix to preseve order for where clause
    prefixOpenlist["a_pre_subject"] = subject;
    prefixOpenlist["b_pre_predicate"] = predicate;
    prefixOpenlist["c_pre_object"] = object;

    Bindings::const_iterator cit = bindings.begin();
    for(; cit != bindings.end(); ++cit)
    {
        std::string binding = *cit;
        select += " ?" + binding + " ";
    }

    where += " {";
    std::map<std::string,std::string> prefixClosedlist;
    std::map<std::string, std::string>::const_iterator pit = prefixOpenlist.begin();
    for(; pit != prefixOpenlist.end(); ++pit)
    {
        try {
            vocabulary::SplitUri splitUri = vocabulary::Utils::extractBaseUri(pit->second);

            // is a full uri
            prefixClosedlist[pit->first] = splitUri.baseUri;
            where += " " + pit->first + ":" + splitUri.name;
        } catch(...)
        {
            // is a placeholder
            where += " " + pit->second;
        }
    }
    where += " . }";


    std::string prefixes;
    std::map<std::string,std::string>::const_iterator pcit = prefixClosedlist.begin();
    for(; pcit != prefixClosedlist.end(); ++pcit)
    {
        prefixes += "PREFIX " + pcit->first + ":<" + pcit->second + ">\n";
    }
    prefixes += " ";

    std::string fullQuery = prefixes + select + where;

    LOG_WARN("query: %s", fullQuery.c_str());

    return query(fullQuery, bindings);
}

Uri Utils::identifyPlaceholder(const Uri& uri, Bindings* bindings)
{
    boost::regex singleVal("[a-z]$");
    boost::regex placeholderVal("\\?[a-z]$");

    if(boost::regex_match(uri,singleVal))
    {
        bindings->push_back(uri);
        return uri;
    } else if(boost::regex_match(uri, placeholderVal) )
    {
        bindings->push_back(uri.substr(1,1));
        return uri;
    }

    // returning unchanges since it is no placeholder
    return uri;
}


} // end namespace query
} // end namespace db
} // end namespace owl_om
