#include "Types.hpp"

namespace organization_model {
namespace activity {

std::map<Type, std::string> TypeTxt = {
    { UNKNOWN, "unknown"},
    { TRANSPORT, "transport" },
    { RECONFIGURATION, "reconfiguration" },
    { ACTIVE_WAIT, "active-wait" },
    { ACTIVE_OPERATIVE, "active-operative" },
    { DORMANT, "dormant"}
};

} // end namespace activity
} // end namespace organization_model
