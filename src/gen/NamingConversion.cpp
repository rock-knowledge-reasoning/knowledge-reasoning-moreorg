#include "NamingConversion.hpp"
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/xpressive/xpressive.hpp>

namespace owl_om
{

std::string CanonizedName::getNamespace() const
{
    std::string ns;
    NamespaceList::const_iterator cit = mNamespaces.begin();
    for(; cit != mNamespaces.end();)
    {
        ns += *cit;
        if(++cit != mNamespaces.end())
            ns += "::";
    }
    return ns;
}

std::string NamingConversion::snakeCase(const std::string& name)
{
    std::string updated = name;
    using namespace boost::xpressive;
    {
        // regex from http://vim.wikia.com/wiki/Converting_variables_to_or_from_camel_case
        sregex r = sregex::compile("([a-z])([A-Z])");
        // from Perl syntax: http://boost-sandbox.sourceforge.net/libs/xpressive/doc/html/boost_xpressive/user_s_guide/string_substitutions.html
        updated = regex_replace(updated,r,"$1_$2");
    }
    {
        sregex r = sregex::compile("-");
        updated = regex_replace(updated,r,"_");
    }
    boost::to_lower(updated);
    return updated;
}

std::string NamingConversion::camelCase(const std::string& name)
{
    if(name.empty())
    {
        return name;
    }

    std::string updated = name;
    using namespace boost::xpressive;
    // regex from http://vim.wikia.com/wiki/Converting_variables_to_or_from_camel_case
    {
        sregex r = sregex::compile("([a-zA-Z])_([a-zA-Z])");
        // from Perl syntax: http://boost-sandbox.sourceforge.net/libs/xpressive/doc/html/boost_xpressive/user_s_guide/string_substitutions.html
        updated = regex_replace(updated,r,"\\l$1\\u$2",regex_constants::format_perl);
    }
    // capitalize
    updated[0] = std::toupper( updated[0] );
    return updated;
}

CanonizedName NamingConversion::canonizeClassName(const std::string& uri)
{
    using namespace boost::xpressive;
    // for help to analyse see: http://regexpal.com/
    //boost::regex r("http://([^\.]+\.\)?([^\.]+)\.([^/]+)/(.*/)?([^\.]*#)?(.*)$");
    sregex r = sregex::compile("http://([^\\.]+\\.)*(?P<baseuri>[^\\.]+)\\.[^/\\.]+/(.*/)*((?P<namespace>[^\\.]*)#)?(?P<classname>.*)$");
    smatch what;

    NamespaceList namespaceList;
    std::string classname;
    if(regex_search(uri,what,r))
    {
        std::string baseuri(what["baseuri"].first, what["baseuri"].second);
        std::string ns(what["namespace"].first, what["namespace"].second);
        classname = std::string(what["classname"].first, what["classname"].second);

        if(!baseuri.empty())
        {
            namespaceList.push_back( snakeCase(baseuri) );
        }

        if(!ns.empty())
        {
            namespaceList.push_back( snakeCase(ns) );
        }
    }


    return CanonizedName( camelCase(classname), namespaceList );
}

} // end namespace owl_om
