#ifndef MOREORG_RESOURCE_INSTANCE_HPP
#define MOREORG_RESOURCE_INSTANCE_HPP

#include "AtomicAgent.hpp"
#include "SharedPtr.hpp"

namespace moreorg {

class ResourceInstance
{
public:
    typedef shared_ptr<ResourceInstance> Ptr;
    typedef std::vector<ResourceInstance> List;
    typedef std::vector<Ptr> PtrList;

    ResourceInstance(const owlapi::model::IRI& name, const owlapi::model::IRI& model);

    const owlapi::model::IRI& getName() const { return mName; }
    const owlapi::model::IRI& getModel() const { return mModel; }

    const AtomicAgent& getAtomicAgent() const { return mAtomicAgent; }
    void setAtomicAgent(const AtomicAgent& aa) { mAtomicAgent = aa; }

    std::string toString(size_t indent = 0) const;

    static std::string toString(const List& list, size_t indent = 0);

private:
    owlapi::model::IRI mName;
    owlapi::model::IRI mModel;

    /// Optionally allow to set the agent this resource is associated with
    AtomicAgent mAtomicAgent;
};


}
#endif // MOREORG_RESOURCE_INSTANCE_HPP
