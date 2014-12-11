#ifndef OWLAPI_EXCEPTIONS_HPP
#define OWLAPI_EXCEPTIONS_HPP

#include <string>
#include <stdexcept>

#define OWLAPI_EXCEPTION(NAME, BASE_MSG) \
    class NAME : public std::runtime_error {\
            public: \
                           NAME() : std::runtime_error(BASE_MSG) \
               {}; \
               NAME(const std::string& msg) : std::runtime_error(BASE_MSG + msg) \
               {}; \
           };

namespace owlapi {
    OWLAPI_EXCEPTION(NotImplemented, "owlapi: Functionality has not been implemented: ");
    OWLAPI_EXCEPTION(NotSupported, "owlapi: Not supported: ");
} // end namespace owl_om
#endif // OWLAPI_EXCEPTIONS_HPP
