#ifndef OWL_OM_INSTANCE_HPP
#define OWL_OM_INSTANCE_HPP

#include <owl_om/Class.hpp>

namespace owl_om {

class Instance
{
public:
    typedef boost::shared_ptr<Instance> Ptr;
    typedef std::vector<Instance::Ptr> List;

    Instance(const Name& name, Class::Ptr klass);

    Name getName() const { return mName; }

    std::string toString() const { return mName + " " + mpKlass->getName(); }

private:
    Name mName;
    Class::Ptr mpKlass;
};

} // end namespace owl_om
#endif // OWL_OM_INSTANCE_HPP
