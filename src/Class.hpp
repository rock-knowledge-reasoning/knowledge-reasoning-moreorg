#ifndef OWL_OM_CLASS_HPP
#define OWL_OM_CLASS_HPP

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <owl_om/Vocabulary.hpp>

namespace owl_om
{

class Property;

typedef Uri Name;


/**
 * \class Class
 * \brief Describes the model of an owl::Class
 * \details Allow to represent a schema in an object oriented manner
 * Should inherit from rdfs:Resource and rdfs:Class
 * \see http://www.infowebml.ws/rdf-owl/graphical-representations.htm#rdfsClass as reference
 */
class Class
{
public:
    typedef boost::shared_ptr<Class> Ptr;

    typedef std::vector<Class::Ptr> List;
    typedef std::map<Name, Class::Ptr> Map;

    Class() {}

    Class(const Name& name);

    virtual ~Class() {}

    void addSuperClass(Class::Ptr klass);

    void addProperty(boost::shared_ptr<Property> property);

    /**
     * Check if class is
     * \return if class is of the same class or a subclass
     */
    bool subclassOf(const Class& klass) const;

    /**
     * Check if class is
     * \return if class is of the same class or a subclass
     */
    bool subclassOf(const Name& klassName) const;

    /**
     * Check if class has property
     * \return true
     */
    bool hasProperty(const Property& propertyName) const;

    /**
     * Check if class has property of given name
     * \return true
     */
    bool hasProperty(const Name& propertyName) const;

    /**
     * Get the type
     * \return type name of this object
     */
    virtual std::string type() const { return vocabulary::OWL::Class(); }

    /**
     * Get name of this class
     */
    virtual Name getName() const { return mName; }

    /**
     * Equals operator for class type
     * \return true if classes are the same
     */
    bool operator==(const Class& other) { return this->getName() == other.getName(); }

    /**
     * Check if class inherits from other
     * \return true if classes are the same
     */
    bool operator<(const Class& other) { return this->subclassOf(other); }

    /**
     * Check if class is same or inherits
     * \return true if classes are the same
     */
    bool operator<=(const Class& other) { return *this == other || *this < other; }

    /**
     * Convert to string
     * \return String representation of class
     */
    std::string toString() const;

private:
    Name mName;
    Map mSuperClasses;
    std::map<Name, boost::shared_ptr<Property> > mProperties;
};

} // end namespace owl_om
#endif // OWL_OM_CLASS_HPP
