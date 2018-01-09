#ifndef ORGANIZATION_MODEL_TYPES_HPP
#define ORGANIZATION_MODEL_TYPES_HPP

#include <map>
#include <string>

namespace organization_model {
namespace activity {

    enum Type { UNKNOWN,
        /// Transport between two locations
        TRANSPORT,
        /// Reconfiguration
        RECONFIGURATION,
        /// Active, but waiting for other agent(s) to complete tasks in order
        /// to procees
        ACTIVE_WAIT,
        /// Actively performing operation
        ACTIVE_OPERATIVE,
        /// Not an active or even assembled agent
        DORMANT
    };

    extern std::map<Type, std::string> TypeTxt;

} // end namespace activity
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_TYPES_HPP
