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

}
