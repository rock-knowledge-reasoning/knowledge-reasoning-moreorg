#include "Resource.hpp"
#include <algorithm>
#include <base-logging/Logging.hpp>
#include <sstream>

namespace organization_model {

Resource::Resource(const owlapi::model::IRI& model)
    : mModel(model)
{}

Resource::Resource(const owlapi::model::IRI& model,
        const PropertyConstraint::Set& propertyConstraints)
    : mModel(model)
    , mPropertyConstraints(propertyConstraints)
{
}

Resource::Resource(const owlapi::model::IRI& model,
        const PropertyConstraint::List& propertyConstraints)
    : mModel(model)
    , mPropertyConstraints(propertyConstraints.begin(), propertyConstraints.end())
{
}

Resource::~Resource() {}

bool Resource::operator<(const Resource& other) const
{
    if(mModel < other.mModel)
    {
        return true;
    } else if(mModel == other.mModel)
    {
        if(mPropertyConstraints < other.mPropertyConstraints)
        {
            return  true;
        } else {
            return false;
        }
    }
    return false;
}

Resource::Set Resource::toResourceSet(const owlapi::model::IRIList& models)
{
    Resource::Set resources;
    {
        owlapi::model::IRIList::const_iterator cit = models.begin();
        for(; cit != models.end(); ++cit)
        {
            resources.insert( Resource(*cit) );
        }
    }
    return resources;
}

void Resource::addPropertyConstraint(const PropertyConstraint& constraint)
{
    mPropertyConstraints.insert(constraint);
}

void Resource::addPropertyConstraints(const PropertyConstraint::List& constraints)
{
    for(const PropertyConstraint& c : constraints)
    {
        addPropertyConstraint(c);
    }
}

void Resource::addPropertyConstraints(const PropertyConstraint::Set& constraints)
{
    for(const PropertyConstraint& c : constraints)
    {
        addPropertyConstraint(c);
    }
}

std::string Resource::toString(size_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ss << hspace << getModel().toQuotedString() << std::endl;
    ss << hspace << "    Constraints:" << std::endl;
    for(const PropertyConstraint& constraint : mPropertyConstraints)
    {
        ss << hspace << "        " << constraint.toString();
    }
    return ss.str();
}

std::string Resource::toString(const Resource::List& resources, size_t indent)
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    for(const Resource& r : resources)
    {
        ss << r.toString(indent) << std::endl;
    }
    return ss.str();
}

std::string Resource::toString(const Resource::Set& resources, size_t indent)
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    for(const Resource& r : resources)
    {
        ss << r.toString(indent) << std::endl;
    }
    return ss.str();
}

void Resource::merge(const Resource& other)
{
    if(mModel != other.mModel)
    {
        throw std::invalid_argument("organization_model::Resource::merge: cannot merge resources: "
                " model are different: '" + mModel.toString() + "' and '" + other.mModel.toString());
    }

    addPropertyConstraints(other.getPropertyConstraints());
}

Resource Resource::merge(const Resource& a, const Resource& b)
{
    if(a.mModel != b.mModel)
    {
        throw std::invalid_argument("organization_model::Resource::merge: cannot merge resources: "
                " model are different: '" + a.mModel.toString() + "' and '" + b.mModel.toString());
    }

    Resource resource = a;
    resource.addPropertyConstraints(b.getPropertyConstraints());
    return resource;
}

Resource::Set Resource::merge(const Resource::Set& resources)
{
    Resource::Set mergedResources;
    Resource::Set remainingResources = resources;

    Resource::Set::const_iterator ait = remainingResources.begin();
    // Using std::distance function here, to make sure to stop for loop,
    // when second last, or last element have been hit
    // (last one can happen due to erase operation in the for loop
    for(; std::distance(ait,remainingResources.end()) > 1; ++ait)
    {
        Resource a = *ait;

        Resource::Set::iterator bit;
        for(bit = std::next(ait,1); bit != remainingResources.end();)
        {
            const Resource& b = *bit;

            if(a.mModel == b.mModel)
            {
                a.merge(b);
                remainingResources.erase(bit++);
            } else
            {
                ++bit;
            }
        }
        mergedResources.insert(a);
    }
    return remainingResources;
}

Resource::Set Resource::merge(const Resource::Set& resourcesA, const Resource::Set& resourcesB)
{
    Resource::Set resources = resourcesA;
    resources.insert(resourcesB.begin(), resourcesB.end());
    return Resource::merge(resources);
}

}
