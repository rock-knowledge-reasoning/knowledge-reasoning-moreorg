#include "Vocabulary.hpp"

namespace owl_om {
namespace vocabulary {

SplitUri Utils::extractBaseUri(const Uri& uri)
{
    size_t found = uri.find_last_of("#");
    return SplitUri(uri.substr(0,found+1), uri.substr(found+1));
}

} // end namespace vocabulary
} // end namespace owl
