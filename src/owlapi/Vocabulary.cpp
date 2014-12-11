#include "Vocabulary.hpp"
#include <stdexcept>

namespace owlapi {
namespace vocabulary {

SplitUri Utils::extractBaseUri(const Uri& uri)
{
    size_t positionSlash = uri.find_last_of("/");
    size_t positionHash = uri.find_last_of("#");
    if(positionHash != std::string::npos)
    {
        if(positionHash < positionSlash)
        {
            throw std::runtime_error("owl_om::vocabulary::Utils extractBaseUri failed: slash is following a hash");
        } else {
            return SplitUri(uri.substr(0,positionHash+1), uri.substr(positionHash+1));
        }
    }
    return SplitUri(uri.substr(0,positionSlash+1), uri.substr(positionSlash+1));
}

} // end namespace vocabulary
} // end namespace owlapi
