#ifndef OWL_OM_PROPERTY_HPP
#define OWL_OM_PROPERTY_HPP

#include <vector>
#include <owl_om/Vocabulary.hpp>
#include <owl_om/Class.hpp>

namespace owl_om
{

/**
 * \class Property
 * \brief Default class for owl:Property
 */
class Property : public Class
{
protected:
    Property() {}

public:
    friend class Registry;

    typedef boost::shared_ptr<Property> Ptr;

    typedef std::map<Name, Property::Ptr> Map;
    typedef std::vector<Property::Ptr> List;

    Property(const Name& name)
        : Class(name)
    {}

    virtual std::string type() const { return vocabulary::RDF::Property(); }
};

/**
 * \class ObjectProperty
 * \brief Default class for owl:ObjectProperty
 */
class ObjectProperty : public Property
{
protected:
    ObjectProperty() {}

public:
    friend class Registry;

    ObjectProperty(const Name& name)
        : Property(name)
    {}

    virtual std::string type() const { return vocabulary::OWL::ObjectProperty(); }

};


/**
 * \class DatatypeProperty
 * \brief Default class for owl:DatatypeProperty
 */
class DatatypeProperty : public Property
{
protected:
    DatatypeProperty() {}

public:
    friend class Registry;

    DatatypeProperty(const Name& name)
        : Property(name)
    {}

    virtual std::string type() const { return vocabulary::OWL::DatatypeProperty(); }

};

/**
 * \class AnnotationProperty
 * \brief Default class for owl:AnnotationProperty
 */
class AnnotationProperty : public Property
{
protected:
    AnnotationProperty() {}

public:
    friend class Registry;

    AnnotationProperty(const Name& name)
        : Property(name)
    {}

    virtual std::string type() const { return vocabulary::OWL::AnnotationProperty(); }
};

} // end namespace om
#endif // OWL_OM_PROPERTY_HPP
