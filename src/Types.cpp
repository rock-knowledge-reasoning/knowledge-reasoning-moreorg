#include "Types.hpp"

namespace moreorg {
namespace activity {

std::map<Type, std::string> TypeTxt = {
    {UNKNOWN, "unknown"},
    {TRANSPORT, "transport"},
    {RECONFIGURATION, "reconfiguration"},
    {IDLE, "idle"},
    {BUSY, "busy"},
};

} // end namespace activity
} // end namespace moreorg
