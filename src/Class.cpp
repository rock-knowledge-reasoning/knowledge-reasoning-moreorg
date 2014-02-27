#include "Class.hpp"
#include "Registry.hpp"

#include <stdexcept>
#include <owl_om/Property.hpp>

namespace owl_om {

Class::Class(const Name& name)
    : mName(name)
{}

void Class::addSuperClass(Class::Ptr klass)
{
    assert(klass);

    if(!subclassOf(klass->getName()))
    {
        mSuperClasses[klass->getName()] = klass;
    }
}

void Class::addProperty(Property::Ptr property)
{
    assert(property);

    if(!hasProperty(property->getName()))
    {
        mProperties[property->getName()] = property;
    }
}

bool Class::subclassOf(const Class& klass) const
{
    return subclassOf(klass.getName());
}

bool Class::subclassOf(const Name& klassName) const
{
    Map::const_iterator cit = mSuperClasses.begin();
    for(; cit != mSuperClasses.end(); ++cit)
    {
        if(cit->first == klassName)
        {
            return true;
        }

        // Transitive property, i.e. if superklass is subclass of A
        // so is this class
        if(cit->second->subclassOf(klassName))
        {
            return true;
        }
    }
    return false;
}

bool Class::hasProperty(const Name& propertyName) const
{
    Property::Map::const_iterator cit = mProperties.find( propertyName );
    if(cit != mProperties.end())
    {
        return true;
    }
    return false;
}

Property::Ptr Class::getProperty(const Name& propertyName) const
{
    Property::Map::const_iterator cit = mProperties.find( propertyName );
    if(cit != mProperties.end())
    {
        return cit->second;
    }
    std::string msg = "owl_om::Property: property '" + propertyName + "' does not exist for class '" + getName() + "'";
    throw std::runtime_error(msg);
}

std::string Class::toString() const
{
    std::string txt = "class " + getName() + "\n";
    txt += "    superclasses: \n";
    Class::Map::const_iterator sit = mSuperClasses.begin();
    for(; sit != mSuperClasses.end(); ++sit)
    {
        txt += "        " + sit->first + "\n";
    }

    txt += "    properties: \n";
    Property::Map::const_iterator pit = mProperties.begin();
    for(; pit != mProperties.end(); ++pit)
    {
        txt += "        " + pit->first + "\n";
    }

    return txt;
}

} // end namespace owl_om
