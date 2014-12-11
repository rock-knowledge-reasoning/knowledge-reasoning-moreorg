#include "OWLLiteral.hpp"
#include "OWLLiteralInteger.hpp"
#include "OWLLiteralDouble.hpp"
#include <owl_om/owlapi/Vocabulary.hpp>
#include <boost/lexical_cast.hpp>

namespace owlapi {
namespace model {

OWLLiteral::OWLLiteral(const std::string& value)
{
    size_t pos = value.find("^^");
    if(std::string::npos != pos)
    {
        mValue = value.substr(0, pos);
        mType = value.substr(pos+2);
    } else {
        mValue = value;
    }
}

bool OWLLiteral::isTyped() const
{
    return !mType.empty();
}

bool OWLLiteral::hasType(const IRI& typeIRI) const
{
    return typeIRI.toString() == mType;
}

std::string OWLLiteral::toString() const
{
    return mValue + "^^" + mType;
}

OWLLiteral::Ptr OWLLiteral::create(const std::string& literal)
{
    OWLLiteral testLiteral(literal);
    IRI type = testLiteral.mType;
    if(type == vocabulary::XSD::integer() || type == vocabulary::XSD::nonNegativeInteger() || type == vocabulary::XSD::resolve("int"))
    {
        return OWLLiteral::Ptr(new OWLLiteralInteger(literal));
    } else if(type == vocabulary::XSD::resolve("double") )
    {
        return OWLLiteral::Ptr(new OWLLiteralDouble(literal));
    } else {
        return OWLLiteral::Ptr(new OWLLiteral(literal));
    }
}

int OWLLiteral::getInteger() const
{
    try {
        return boost::lexical_cast<int>(mValue);
    } catch(const std::bad_cast& e)
    {
        throw std::runtime_error("OWLLiteral::getInteger not implemented for '" + mValue + "' and given type: '" + mType + "'");
    }
}

double OWLLiteral::getDouble() const
{
    try {
        return boost::lexical_cast<double>(mValue);
    } catch(const std::bad_cast& e)
    {
        throw std::runtime_error("OWLLiteral::getDouble not implemented for '" + mValue + "' and given type: '" + mType + "'");
    }
}

} // end namespace model
} // end namespace owlapi
