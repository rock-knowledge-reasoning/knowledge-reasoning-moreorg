#include "OWLLiteral.hpp"
#include "OWLLiteralInteger.hpp"
#include <owl_om/Vocabulary.hpp>
#include <boost/lexical_cast.hpp>

using namespace owl_om;

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

} // end namespace model
} // end namespace owlapi
