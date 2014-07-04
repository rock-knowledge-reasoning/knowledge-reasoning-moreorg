#include "IRI.hpp"

#include <ctype.h>
#include <owl_om/owlapi/io/XMLUtils.hpp>

namespace owlapi {
namespace model {

IRI::IRI(const std::string& prefix, const std::string& remainder)
    : mPrefix(prefix)
    , mRemainder(remainder)
{}

URI IRI::toURI() const
{
    return URI::create(mPrefix + mRemainder);
}

bool IRI::isAbsolute() const
{
    if( std::string::npos == mPrefix.find(":"))
    {
        return false;
    }

    for(size_t i = 0; i < mPrefix.size(); ++i)
    {
        char ch = mPrefix.at(i);
        if( !(isalpha(ch) || !isdigit(ch) || ch == '.' ||
                    ch == '+' || ch == '-'))
        {
            return false;
        }
    }

    return true;
}

std::string IRI::getScheme() const
{
    size_t pos = mPrefix.find(":");
    if( std::string::npos == pos)
    {
        throw std::invalid_argument("IRI::getScheme iri does not have schema");
    }

    return mPrefix.substr(0, pos);
}

IRI IRI::resolve(const std::string& iri_string) const
{
    URI uri = URI::create(iri_string);
    if(uri.isAbsolute() || uri.isOpaque())
    {
        return create(uri);
    }

//    return create(toURI().resolve(uri));
    throw std::runtime_error("Not implemented: IRI::resolve");
}

std::string IRI::toQuotedString() const
{
    return '<' + mPrefix + mRemainder + '>';
}

IRI IRI::create(const std::string& iri_string)
{
    if(iri_string.empty())
    {
        throw std::invalid_argument("IRI::create iri cannot created from empty string");
    }

    size_t splitPos = owlapi::io::XMLUtils::getNCNameSuffixIndex(iri_string);
    if(splitPos == std::string::npos)
    {
        return IRI(iri_string, "");
    }

    return IRI(iri_string.substr(0,splitPos), iri_string.substr(splitPos));
}



} // end namespace model
} // end namespace owlapi
