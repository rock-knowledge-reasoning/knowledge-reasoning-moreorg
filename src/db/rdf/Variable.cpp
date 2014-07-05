#include "Variable.hpp"
#include <base/Logging.hpp>

namespace owl_om {
namespace db {
namespace query {

Variable::Variable()
    : IRI()
{}

Variable::Variable(const IRI& iri)
    : IRI(iri)
{
}

Variable::Variable(const std::string& name, bool grounded)
    : IRI(name)
{
    if(!grounded)
    {
        if(name.find("http://") != std::string::npos)
        {
            throw std::invalid_argument("owl_om::Query::unboundVariable: given URI: '" + name + "'");
        }

        if(name.data()[0] != '?')
        {
            mPrefix = "?" + name;
        } else {
            mPrefix = name;
        }
    }
}

std::string Variable::getName() const
{
    std::string fullTxt = toString();
    if(isGrounded())
    {
        return fullTxt;
    } else {
        if(fullTxt.size() > 1)
        {
            return toString().substr(1);
        } else {
            LOG_WARN_S << "Empty name for variable";
            return std::string();
        }
    }
}

std::string Variable::getQueryName() const
{
    if(isGrounded())
    {
        return toQuotedString();
    } else {
        return toString();
    }
}

bool Variable::isGrounded() const
{ 
    return mPrefix.empty() || mPrefix[0] != '?';
}

} // end namespace query
} // end namespace db
} // end namespace owl_om
