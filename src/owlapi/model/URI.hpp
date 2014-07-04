#ifndef OWL_OM_OWL_API_MODEL_URI_HPP
#define OWL_OM_OWL_API_MODEL_URI_HPP

#include <string>

namespace owlapi {
namespace model {

class URI
{
    std::string mUri;

    URI(const std::string& s)
        : mUri(s)
    {}

public:
    static URI create(const std::string& s) { return URI(s); }

    bool isAbsolute() const { return !mUri.empty() && mUri.at(0) != '/'; }

    bool isOpaque() const { return false; }

};

} // end namespace model
} // end namespace owlapi
#endif // OWL_OM_OWL_API_MODEL_URI_HPP
