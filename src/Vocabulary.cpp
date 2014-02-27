#include "Vocabulary.hpp"
#include <stdexcept>

namespace owl_om {
namespace vocabulary {

SplitUri Utils::extractBaseUri(const Uri& uri)
{
    size_t found = uri.find_last_of("#");
    if(found == std::string::npos)
    {
        throw std::runtime_error("owl_om::vocabulary::Utils extractBaseUri failed");
    }

    return SplitUri(uri.substr(0,found+1), uri.substr(found+1));
}

} // end namespace vocabulary
} // end namespace owl
