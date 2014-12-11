#ifndef OWLAPI_MODEL_OWL_LITERAL_DOUBLE_HPP
#define OWLAPI_MODEL_OWL_LITERAL_DOUBLE_HPP

#include <owl_om/owlapi/model/OWLLiteral.hpp>

namespace owlapi {
namespace model {

class OWLLiteralDouble : public OWLLiteral
{
public:
    OWLLiteralDouble(double value);

    OWLLiteralDouble(const std::string& value);

    double getDouble() const;
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_LITERAL_DOUBLE_HPP
