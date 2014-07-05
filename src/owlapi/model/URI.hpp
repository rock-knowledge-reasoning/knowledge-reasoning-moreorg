#ifndef OWL_OM_OWL_API_MODEL_URI_HPP
#define OWL_OM_OWL_API_MODEL_URI_HPP

#include <string>
#include <map>
#include <stdexcept>

struct UriUriStructA;

namespace owlapi {
namespace model {

class URI
{
    UriUriStructA* mUri;

    static std::map<int, std::string> ErrorCodeTxt;

public:
    URI();

    ~URI();

    URI(const std::string& s);

    /**
     * Test if URI is absolute
     */
    bool isAbsolute() const;

    /**
     * Test if URI is opaque
     */
    bool isOpaque() const;

    /**
     * Resolve the provided uri
     * \return resolve uri
     */
    URI resolve(const URI& relativeUri) const;

    void normalize();

    /**
     * Get string representation of URI
     * \return URI as string
     */
    std::string toString() const;
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_OM_OWL_API_MODEL_URI_HPP
