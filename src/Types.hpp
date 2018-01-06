#ifndef ORGANIZATION_MODEL_TYPES_HPP
#define ORGANIZATION_MODEL_TYPES_HPP

namespace organization_model {
namespace activity {

    enum Type { UNKOWN,
        RECONFIGURATION,
        ACTIVE_WAIT,
        ACTIVE_OPERATIVE,
        DORMANT
    };

} // end namespace activity
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_TYPES_HPP
