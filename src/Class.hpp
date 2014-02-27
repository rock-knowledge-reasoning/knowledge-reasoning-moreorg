#ifndef OWL_OM_CLASS_HPP
#define OWL_OM_CLASS_HPP

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <owl_om/Vocabulary.hpp>

/**
 *!
 * \mainpage OWL Object Model (OM)
 * This library intends to give access to owl based definitions in an intuitive way for
 * any programmer familiar with object oriented modelling.
 *
 * While OWL/RDFS allows to define class and property relationship using the TBox model,
 * this library provides a registry for the TBox model.
 *
 *   --------------   |                    |       -----------------
 *  |    SPARQL    |  | BackendAbstraction | >>   | Registry (TBox) |
 *  |   Queries    |  | (File, DB, Stream) | <<   |                 |
 *   -------------    |                    |       -----------------
 *         /\
 *         ||
 *         \/
 *   --------------
 *  |   Reasoner   |   Reasoner is currently considered optional
 *  |   (Fact++)   |
 *   --------------
 *         /\
 *         ||
 *         \/
 *   --------------
 *  |   OWL / RDF  |
 *  |   Database   |
 *   --------------
 *
 */
namespace owl_om
{

class Property;

typedef Uri Name;
typedef std::vector<Name> NameList;


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

    /**
     * \brief Default constructor
     */
    Class() {}

    /**
     * \brief Constructor for (named) class instances
     * \param name Name of class
     */
    Class(const Name& name);

    /**
     * \brief Deconstructor which allow to be overriden by subclass
     */
    virtual ~Class() {}

    /**
     * \brief Add super class
     * \param klass Pointer to class object
     */
    void addSuperClass(Class::Ptr klass);

    /**
     * \brief Get ancestors
     */
    Map getAncestors() const { return mSuperClasses; }

    /**
     * \brief Add a property to this class type
     * \param property Pointer to property object
     */
    void addProperty(boost::shared_ptr<Property> property);

    /**
     * \brief Check if class is a subclass of the given class
     * \return true if class is of the same class or a subclass
     */
    bool subclassOf(const Class& klass) const;

    /**
     * \brief Check if class is a subclass of a class with the given name
     * \return true if class is of the same class or a subclass
     */
    bool subclassOf(const Name& klassName) const;

    /**
     * \brief Check if class has property of given name
     * \param
     * \return true
     */
    bool hasProperty(const Name& propertyName) const;

    /**
     * \brief Get property of the given name
     * \param propertyName Name of the property to retrieve
     * \return Property pointer
     */
    boost::shared_ptr<Property> getProperty(const Name& propertyName) const;

    /**
     * \brief Get name of this class
     */
    virtual Name getName() const { return mName; }

    /**
     * \brief Equals operator for class type
     * \return true if classes are the same
     */
    bool operator==(const Class& other) { return this->getName() == other.getName(); }

    /**
     * \brief Check if class inherits from other
     * \return true if classes are the same
     */
    bool operator<(const Class& other) { return this->subclassOf(other); }

    /**
     * \brief Check if class is same or inherits
     * \return true if classes are the same
     */
    bool operator<=(const Class& other) { return *this == other || *this < other; }

    /**
     * \brief Convert class to string representation
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
