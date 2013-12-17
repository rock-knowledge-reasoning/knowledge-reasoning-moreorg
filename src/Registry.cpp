#include "Registry.hpp"
#include <stdexcept>
#include <base/Logging.hpp>

namespace owl_om
{

Registry::Registry()
{
    // The class that all others will subclass
    Class::Ptr thing( new Class( vocabulary::OWL::Thing() ) );
    registerClass(vocabulary::OWL::Thing(), thing);

    // owl:Nothing subclasses all other classes
    Class::Ptr nothing(new Class( vocabulary::OWL::Nothing() ) );
    //// add explitely since it will not have been added (due to reverse dependency)
    nothing->addSuperClass(thing);
    registerClass(vocabulary::OWL::Nothing(), nothing );

    Property::Ptr property(new Property());
    registerProperty(vocabulary::RDF::Property(), property);

    Property::Ptr datatypeProperty(new DatatypeProperty());
    registerProperty(vocabulary::OWL::DatatypeProperty(), datatypeProperty);

    Property::Ptr objectProperty(new ObjectProperty());
    registerProperty(vocabulary::OWL::ObjectProperty(), objectProperty);

    Property::Ptr annotationProperty(new AnnotationProperty());
    registerProperty(vocabulary::OWL::AnnotationProperty(), annotationProperty);
}

Class::Ptr Registry::getClass(const Name& name) const
{
    Class::Map::const_iterator cit = mClasses.find(name);
    for(; cit != mClasses.end(); ++cit)
    {
        return cit->second;
    }

    std::string msg = "owl_om::Registry: cannot retrieve class. No class '" + name + "' known";
    throw std::runtime_error(msg);
}

void Registry::registerClass(const Name& name, Class::Ptr klass)
{
    LOG_DEBUG_S << name;

    if(mClasses.find(name) != mClasses.end())
    {
        std::string msg = "owl_om::Registry: class '" + name + "' already registered";
        throw std::runtime_error(msg);
    }
    mClasses[name] = klass; 

    if( name == vocabulary::OWL::Thing() || name == vocabulary::OWL::Nothing())
    {
        return; 
    }

    // every class (except for owl:Thing itself) is a subclass of owl:Thing
    if( !klass->subclassOf( vocabulary::OWL::Thing() ) && name != vocabulary::OWL::Thing())
    {
        klass->addSuperClass( mClasses[vocabulary::OWL::Thing() ] );
    }

    //// owl:Nothing subclasses every class
    Class::Ptr nothing = mClasses[ vocabulary::OWL::Nothing() ];
    if(!nothing->subclassOf(name) && name != vocabulary::OWL::Nothing() )
    {
        nothing->addSuperClass( klass );
    }
}

Property::Ptr Registry::getProperty(const Name& name) const
{
    Property::Map::const_iterator cit = mProperties.find(name);
    for(; cit != mProperties.end(); ++cit)
    {
        return cit->second;
    }

    std::string msg = "owl_om::Registry: cannot retrieve property. No property '" + name + "' known";
    throw std::runtime_error(msg);
}

void Registry::registerProperty(const Name& name, Property::Ptr klass)
{
    if(mProperties.find(name) != mProperties.end())
    {
        std::string msg = "owl_om::Registry: property for '" + name + "' already registered";
        throw std::runtime_error(msg);
    }
    mProperties[name] = klass; 

    // every property is a subclass of owl::Thing
    if( !klass->subclassOf( vocabulary::OWL::Thing() ) )
    {
        klass->addSuperClass( mClasses[vocabulary::OWL::Thing() ] );
    }
}

std::string Registry::toString() const
{
    Class::Map::const_iterator cit = mClasses.begin();
    std::string txt = "owl::Registry:\n";

    for(; cit != mClasses.end(); ++cit)
    {
        txt += cit->second->toString() + "\n";
    }

    Property::Map::const_iterator pit = mProperties.begin();
    for(; pit != mProperties.end(); ++pit)
    {
        txt += "property: " + pit->first + "\n";
        txt += "     type: " + pit->second->type() + "\n";
    }
    return txt;
}

}
