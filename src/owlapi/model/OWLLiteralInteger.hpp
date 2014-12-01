#ifndef OWLAPI_MODEL_OWL_LITERAL_INTEGER_HPP
#define OWLAPI_MODEL_OWL_LITERAL_INTEGER_HPP

#include <owl_om/owlapi/model/OWLLiteral.hpp>

namespace owlapi {
namespace model {

class OWLLiteralInteger : public OWLLiteral
{
public:
    OWLLiteralInteger(const std::string& value)
        : OWLLiteral(value)
    {}

    int getInteger() const;
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_LITERAL_INTEGER_HPP
