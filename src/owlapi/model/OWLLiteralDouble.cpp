#include "OWLLiteralDouble.hpp"
#include <boost/lexical_cast.hpp>
#include <sstream>
#include <owl_om/owlapi/Vocabulary.hpp>

namespace owlapi {
namespace model {

OWLLiteralDouble::OWLLiteralDouble(double value)
{
    std::stringstream ss;
    ss << value;

    mValue = ss.str();
    mType = vocabulary::XSD::resolve("double").toString();
}

OWLLiteralDouble::OWLLiteralDouble(const std::string& value)
    : OWLLiteral(value)
{}

double OWLLiteralDouble::getDouble() const
{
    return boost::lexical_cast<double>(mValue);
}

} // end namespace model
} // end namespace owlapi
