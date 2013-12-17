#ifndef OWL_OM_VOCABULARY_HPP
#define OWL_OM_VOCABULARY_HPP

#include <string>

#define VOCABULARY_BASE_URI(URI) \
    static Uri BaseUri() { return URI; }
#define VOCABULARY_ADD_WORD(NAME) \
    static Uri NAME() { return BaseUri() + #NAME; }

namespace owl_om {
    typedef std::string Uri;

namespace vocabulary {

struct SplitUri
{
    SplitUri(const std::string& _baseUri, const std::string& _name)
        : baseUri(_baseUri)
        , name(_name)
    {}

    std::string fullName() { return baseUri + name; }

    std::string baseUri;
    std::string name;
};

class Utils
{
public:
    static SplitUri extractBaseUri(const Uri& uri);
};

} // end namespace vocabulary
} // end namespace owl_om

#include <owl_om/vocabularies/OWL.hpp>
#include <owl_om/vocabularies/RDF.hpp>
#include <owl_om/vocabularies/RDFS.hpp>

#endif // OWL_OM_VOCABULARY_HPP
