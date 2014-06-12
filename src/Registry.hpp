#ifndef OWL_OM_REGISTRY_HPP
#define OWL_OM_REGISTRY_HPP

#include <map>
#include <owl_om/Class.hpp>
#include <owl_om/Property.hpp>

namespace owl_om {

/**
 * Custom TBox representation to allow for an
 * intuitive 'object oriented' handling of OWL based
 * axioms
 */
class Registry
{
public:
    Registry();

    /**
     * Get instance of a certain class model
     */
    Class::Ptr getClass(const Name& name, bool lazy = false);

    /**
     * Register a certain class type for a given uri
     * \param name
     * \param klass
     */
    void registerClass(Class::Ptr klass);

    /**
     * Get map of all classes
     * \return class map
     */
    Class::Map getAllClasses() const { return mClasses; }

    /**
     * Get property
     */
    Property::Ptr getProperty(const Name& name) const;

    /**
     * Add property
     */
    void registerProperty(Property::Ptr klass);

    /**
     * Get all properties
     * \return property map
     */
    Property::Map getAllProperties() const { return mProperties; }

    /**
     * Output content of the registry
     */
    std::string toString() const;

private:
    Class::Map mClasses;
    Property::Map mProperties;
};

} // end namespace owl_om
#endif // OWL_OM_REGISTRY_HPP
