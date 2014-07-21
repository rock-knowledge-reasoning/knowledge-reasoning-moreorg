#include "IRI.hpp"

#include <ctype.h>
#include <owl_om/owlapi/io/XMLUtils.hpp>
#include <base/Logging.hpp>

namespace owlapi {
namespace model {

void IRI::setFromString(const std::string& s)
{
    if(s.empty())
    {
        throw std::invalid_argument("IRI::setFromString iri cannot created from empty string");
    }

    size_t splitPos = owlapi::io::XMLUtils::getNCNameSuffixIndex(s);
    if(splitPos == std::string::npos)
    {
        mPrefix = s;
    } else {
        mPrefix = s.substr(0,splitPos);
        mRemainder = s.substr(splitPos);
    }
}

IRI::IRI(const char* s)
{
    setFromString( std::string(s) );
}

IRI::IRI(const std::string& s)
{
    setFromString(s);
}

IRI::IRI(const std::string& prefix, const std::string& remainder)
    : mPrefix(prefix)
    , mRemainder(remainder)
{}

URI IRI::toURI() const
{
    return URI(mPrefix + mRemainder);
}

bool IRI::isAbsolute() const
{
    size_t colonPos = mPrefix.find(":");
    if( std::string::npos == colonPos )
    {
        return false;
    }

    for(size_t i = 0; i < colonPos; ++i)
    {
        char ch = mPrefix.at(i);
        if( !(isalpha(ch) || isdigit(ch) || ch == '.' ||
                    ch == '+' || ch == '-'))
        {
            LOG_DEBUG_S << "owlapi::model::IRI::isAbsolute: Invalid character in prefix '" << ch << "'";
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

IRI IRI::resolve(const std::string& s) const
{
    URI uri(s);
    if(uri.isAbsolute() || uri.isOpaque())
    {
        return create(uri);
    }

    return IRI::create( toURI().resolve(uri) );
}

std::string IRI::getFragment() const
{
    size_t npos = mRemainder.find_last_of("#");
    if(npos == std::string::npos)
    {
        throw std::invalid_argument("owlapi::model::IRI::getFragment IRI " + toQuotedString() + " does not have a fragment");
    }

    return mRemainder.substr(npos+1);
}

IRI IRI::create(const std::string& s)
{
    if(s.empty())
    {
        LOG_DEBUG_S << "IRI::create iri created from empty string";
        return IRI();
    }

    size_t splitPos = owlapi::io::XMLUtils::getNCNameSuffixIndex(s);
    if(splitPos == std::string::npos)
    {
        return IRI(s, "");
    }

    return IRI(s.substr(0,splitPos), s.substr(splitPos));
}

IRI IRI::create(const std::string& prefix, const std::string& suffix)
{
    if(prefix.empty())
    {
        return create(suffix);
    } else if( suffix.empty() )
    {
        return create(suffix);
    } else {
        size_t prefixIndex = owlapi::io::XMLUtils::getNCNameSuffixIndex(prefix);
        size_t suffixIndex = owlapi::io::XMLUtils::getNCNameSuffixIndex(suffix);

        if(prefixIndex == std::string::npos && suffixIndex == 0)
        {
              // the prefix does not contain an ncname character and there is
              // no illegal character in the suffix
              // the split is therefore correct
              return IRI(prefix, suffix);
        }

        // otherwise the split is wrong; we could obtain the right split by
        // using index and test, but it's just as easy to use the other
        // constructor
        return create(prefix + suffix);
    }
}


bool IRI::operator==(const IRI& other) const
{
    return mPrefix == other.mPrefix && mRemainder == other.mRemainder; 
}

std::ostream& operator<<(std::ostream& os, const owlapi::model::IRI& iri)
{ 
    os << iri.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const IRIList& iris)
{
    IRIList::const_iterator cit = iris.begin();
    os << "[";
    for(; cit != iris.end(); ++cit)
    {
        os << cit->toString();
        if(cit+1 != iris.end())
        {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const IRISet& iris)
{
    IRISet::const_iterator cit = iris.begin();
    os << "[";
    for(; cit != iris.end(); ++cit)
    {
        os << cit->toString();
        os << ", ";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector< IRIList >& iris)
{
    std::vector< IRIList >::const_iterator cit = iris.begin();
    for(; cit != iris.end(); ++cit)
    {
        os << *cit << std::endl;
    }
    return os;
}

} // end namespace model
} // end namespace owlapi
