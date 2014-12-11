#ifndef OWLAPI_MODEL_URI_HPP
#define OWLAPI_MODEL_URI_HPP

#include <string>
#include <map>
#include <stdexcept>

struct UriUriStructA;

namespace owlapi {
namespace model {

class URI
{
    std::string mUri;
    bool mIsAbsolute;
    bool mIsOpaque;

    static std::map<int, std::string> ErrorCodeTxt;

    std::string update(UriUriStructA* uri);

public:
    URI(const std::string& s = "");

    /**
     * Test if URI is absolute
     */
    bool isAbsolute() const { return mIsAbsolute; }

    /**
     * Test if URI is opaque
     */
    bool isOpaque() const { return mIsOpaque; }

    /**
     * Resolve the provided uri
     * \return resolve uri
     */
    URI resolve(const URI& relativeUri) const;

    /**
     * Get string representation of URI
     * \return URI as string
     */
    std::string toString() const { return mUri; }
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_URI_HPP
