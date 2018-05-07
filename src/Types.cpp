#include "Types.hpp"

namespace organization_model {
namespace activity {

std::map<Type, std::string> TypeTxt = {
    { UNKNOWN, "unknown"},
    { TRANSPORT, "transport" },
    { RECONFIGURATION, "reconfiguration" },
    { IDLE, "idle" },
    { BUSY, "busy" },
};

} // end namespace activity
} // end namespace organization_model
