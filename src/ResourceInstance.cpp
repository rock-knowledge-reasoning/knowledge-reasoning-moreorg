#include "ResourceInstance.hpp"
#include <sstream>

namespace moreorg {

ResourceInstance::ResourceInstance(const owlapi::model::IRI& name, const owlapi::model::IRI& model)
    : mName(name)
    , mModel(model)
{
}

std::string ResourceInstance::toString(size_t indent) const
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace << mName.toString() << " [" << mModel.toString() << "] --> ";
    ss << mAtomicAgent.getName();
    return ss.str();
}

std::string ResourceInstance::toString(const List& list, size_t indent)
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ss << hspace << "ResourceInstances:" << std::endl;
    for(const ResourceInstance& instance : list)
    {
        ss << instance.toString(indent + 4) << std::endl;
    }
    return ss.str();
}

} // end namespace moreorg
