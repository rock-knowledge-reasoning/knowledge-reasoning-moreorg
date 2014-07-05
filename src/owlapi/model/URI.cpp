#include "URI.hpp"
// http://uriparser.sourceforge.net/doc/html/structUriParserStateStructA.html
#include <uriparser/Uri.h>
#include <boost/assign/list_of.hpp>

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

URI::URI()
    : mUri(0)
{}

URI::URI(const std::string& s)
    : mUri(new UriUriA())
{
    UriParserStateA state;
    state.uri = mUri;
    if (uriParseUriA(&state, s.c_str()) != URI_SUCCESS) 
    {
       uriFreeUriMembersA(mUri);
       throw std::invalid_argument("owlapi::model::URI: uri" + s + " is invalid: " + ErrorCodeTxt[state.errorCode]);
    }
}

URI::~URI()
{
    uriFreeUriMembersA(mUri);
    delete mUri;
}

bool URI::isAbsolute() const 
{
    return mUri->absolutePath;
}

bool URI::isOpaque() const
{
    return false;
}

URI URI::resolve(const URI& relativeUri) const
{
    URI absoluteDestination(toString() + relativeUri.toString());

    // uriparser seems to be broken in this context, this using simple append and normalize
    // as reduction of the uri
    //
    //if (uriAddBaseUriA(absoluteDestination.mUri, relativeUri.mUri, mUri) != URI_SUCCESS) 
    //{
    //        /* Failure */
    //    uriFreeUriMembersA(absoluteDestination.mUri);
    //    throw std::runtime_error("owlapi::model::URI::resolve uri " + relativeUri.toString() + "' could not be resolved ");
    //}

    absoluteDestination.normalize();
    return absoluteDestination;
}

void URI::normalize()
{
    if (uriNormalizeSyntaxA(mUri) != URI_SUCCESS) 
    {
        throw std::runtime_error("owlapi::model::URI::normalize failed for uri '" + toString() + "'");
    }
}


std::string URI::toString() const
{
    int charsRequired;
    if (uriToStringCharsRequiredA(mUri, &charsRequired) != URI_SUCCESS)
    {
        throw std::runtime_error("owlapi::model::URI::toString() failed since length of uri could not be evaluated");
    }
    charsRequired++;

    char* uriCString = (char*) malloc(charsRequired * sizeof(char));
    if (uriCString == NULL)
    {
        throw std::runtime_error("owlapi::model::URI::toString() failed since not memory could be allocated for string");
    }

    if (uriToStringA(uriCString, mUri, charsRequired, NULL) != URI_SUCCESS)
    {
        throw std::runtime_error("owlapi::model::URI::toString() failed since not memory could be allocated for string");
    }
    std::string uri(uriCString);
    delete uriCString;
    return uri;
}

} // end namespace model
} // end namespace owlapi
