#include "Resource.hpp"
#include <algorithm>
#include <base-logging/Logging.hpp>
#include <sstream>

namespace moreorg {

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

bool Resource::operator==(const Resource& other) const
{
    return mModel == other.mModel && mPropertyConstraints == other.mPropertyConstraints;
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

owlapi::model::IRISet Resource::getModels(const Resource::Set& a)
{
    owlapi::model::IRISet models;
    for(const Resource r : a)
    {
        models.insert(r.getModel());
    }
    return models;
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
    *this = merge(*this, other);
}

Resource Resource::merge(const Resource& a, const Resource& b)
{
    if(a.mModel != b.mModel)
    {
        throw std::invalid_argument("moreorg::Resource::merge: cannot merge resources: "
                " models are different: '" + a.mModel.toString() + "' and '" + b.mModel.toString());
    }

    Resource resource = a;
    resource.addPropertyConstraints(b.getPropertyConstraints());
    return resource;
}

Resource::Set Resource::merge(const Resource::Set& _resources)
{
    Resource::List resources(_resources.begin(), _resources.end());

    for(size_t a = 0; a < resources.size(); ++a)
    {
        Resource& resourceA = resources[a];
        for(size_t b = a+1; b < resources.size();)
        {
            const Resource& resourceB = resources[b];

            if(resourceA.mModel == resourceB.mModel)
            {
                resourceA.merge(resourceB);
                resources.erase(resources.begin() + b);
            } else {
                ++b;
            }
        }
    }
    return Resource::Set(resources.begin(), resources.end());
}

Resource::Set Resource::merge(const Resource::Set& resourcesA, const Resource::Set& resourcesB)
{
    Resource::Set resources = resourcesA;
    resources.insert(resourcesB.begin(), resourcesB.end());
    return Resource::merge(resources);
}

} // end namespace moreorg
