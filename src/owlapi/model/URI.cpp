#include "URI.hpp"
// http://uriparser.sourceforge.net/doc/html/structUriParserStateStructA.html
#include <boost/assign/list_of.hpp>
#include <base/Logging.hpp>
#include <uriparser/Uri.h>

namespace owlapi {
namespace model {

std::map<int, std::string> URI::ErrorCodeTxt = boost::assign::map_list_of
    (URI_SUCCESS,      "Success")
    (URI_ERROR_SYNTAX, "Parsed text violates expected format")
    (URI_ERROR_NULL,   "One of the params passed was NULL although it mustn't be")
    (URI_ERROR_MALLOC, "Requested memory could not be allocated")
    (URI_ERROR_OUTPUT_TOO_LARGE, "Some output is to large for the receiving buffer")
    (URI_ERROR_NOT_IMPLEMENTED, "The called function is not implemented yet")
    (URI_ERROR_RANGE_INVALID, "The parameters passed contained invalid ranges")
    (URI_ERROR_OUTPUT_TOO_LARGE, "URI output too large")
    (URI_ERROR_ADDBASE_REL_BASE, "Add base relative base: given base is not absolute")
    (URI_ERROR_REMOVEBASE_REL_BASE, "Remove base relative base: given base is not absolute")
    (URI_ERROR_REMOVEBASE_REL_SOURCE, "Remove base relative source: given base is not absolute");

URI::URI(const std::string& s)
    : mIsAbsolute(false)
    , mIsOpaque(false)
{
    if(!s.empty())
    {
        UriUriA uri;
        UriParserStateA state;
        state.uri = &uri;
        if (uriParseUriA(&state, s.c_str()) != URI_SUCCESS)
        {
           uriFreeUriMembersA(&uri);
           throw std::invalid_argument("owlapi::model::URI: uri" + s + " is invalid: " + ErrorCodeTxt[state.errorCode]);
        } else {
            mUri = update(&uri);
            uriFreeUriMembersA(&uri);
        }
    }
}

std::string URI::update(UriUriA* uri)
{
    mIsAbsolute = uri->absolutePath;

    // normalize
    if (uriNormalizeSyntaxA(uri) != URI_SUCCESS)
    {
        throw std::runtime_error("owlapi::model::URI::normalize failed");
    }

    int charsRequired = 0;
    if (uriToStringCharsRequiredA(uri, &charsRequired) != URI_SUCCESS)
    {
        throw std::runtime_error("owlapi::model::URI::toString() failed since length of uri could not be evaluated");
    }
    charsRequired++;

    char* uriCString = (char*) calloc(charsRequired, sizeof(char));
    if (uriCString == NULL)
    {
        throw std::runtime_error("owlapi::model::URI::toString() failed since not memory could be allocated for string");
    }

    if (uriToStringA(uriCString, uri, charsRequired, NULL) != URI_SUCCESS)
    {
        throw std::runtime_error("owlapi::model::URI::toString() failed since not memory could be allocated for string");
    }

    return std::string(uriCString);
}

URI URI::resolve(const URI& relativeUri) const
{
    return URI( mUri + relativeUri.mUri );
}

} // end namespace model
} // end namespace owlapi
