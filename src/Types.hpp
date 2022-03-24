#ifndef ORGANIZATION_MODEL_TYPES_HPP
#define ORGANIZATION_MODEL_TYPES_HPP

#include <map>
#include <string>

namespace moreorg {
namespace activity {
enum Type {
    UNKNOWN = 0x00,
    /// The system is idle
    IDLE = 0x01,
    /// Actively performing operation general activity
    BUSY = 0x02,
    /// More detailed activities (sub flags of BUSY)
    /// Transport between two locations
    TRANSPORT = (0x02 & 0x12),
    /// Reconfiguration
    RECONFIGURATION = (0x02 & 0x22),
    /// Not an active or even assembled agent
};

/**
 * Allow checking of activity via
 * if(activity & (TRANSPORT | RECONFIGURATION))
 * {
 * ...
 * }
 */
inline Type operator|(Type a, Type b)
{
    return static_cast<Type>(static_cast<int>(a) | static_cast<int>(b));
}

extern std::map<Type, std::string> TypeTxt;

} // end namespace activity
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_TYPES_HPP
