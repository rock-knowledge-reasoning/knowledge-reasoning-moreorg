#include "Instance.hpp"

namespace owl_om {

Instance::Instance(const Name& name, Class::Ptr klass)
    : mName(name)
    , mpKlass(klass)
{
}

}
