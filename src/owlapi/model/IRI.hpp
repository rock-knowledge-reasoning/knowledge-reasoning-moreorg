#ifndef OWL_OM_OWL_API_MODEL_IRI_HPP
#define OWL_OM_OWL_API_MODEL_IRI_HPP

#include <stdexcept>
#include <owl_om/owlapi/model/URI.hpp>

namespace owlapi {
namespace model {

class IRI
{
    std::string mPrefix;
    std::string mRemainder;

public:
    IRI();

    IRI(const std::string& prefix, const std::string& remainder);

    URI toURI() const;

    /**
     * Test if IRI is absolute
     * \return true upon success, false otherwise
     */
    bool isAbsolute() const;

    /**
     * \return IRI scheme, i.e http, urn
     * \throws if IRI does not have a scheme
     */
    std::string getScheme() const;

    /**
     * Get the prefix (namespace) of this IRI
     */
    std::string getPrefix() const { return mPrefix; }

    /**
     * Get the namespace / prefix of this IRI
     */
    std::string getNamespace() const { return getPrefix(); }

    /**
     * Get remainder
     */
    std::string getRemainder() const { return mRemainder; }

    /**
     * Resolve the IRI
     */
    IRI resolve(const std::string& iri_string) const;

    /**
     * To quoted string
     */
    std::string toQuotedString() const;

    static IRI create(const std::string& iri_string);

    static IRI create(const URI& iri_string) { throw std::runtime_error("IRI not implemented");}
};

} // end namespace model
} // end namespace owlapi
#endif // OWL_OM_OWL_API_MODEL_IRI_HPP
