#ifndef OWL_OM_EXCEPTIONS_HPP
#define OWL_OM_EXCEPTIONS_HPP

#include <string>
#include <stdexcept>

#define OWL_OM_EXCEPTION(NAME, BASE_MSG) \
    class NAME : public std::runtime_error {\
            public: \
                           NAME() : std::runtime_error(BASE_MSG) \
               {}; \
               NAME(const std::string& msg) : std::runtime_error(BASE_MSG + msg) \
               {}; \
           };

namespace owl_om {
    OWL_OM_EXCEPTION(NotImplemented, "owl_om: Functionality has not been implemented: ");
    OWL_OM_EXCEPTION(NotSupported, "owl_om: Not supported: ");
} // end namespace owl_om
#endif // OWL_OM_EXCEPTIONS_HPP
