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

    /**
     * \brief Allow to indentify this property by type(name)
     */
    virtual std::string type() const { return vocabulary::RDF::Property(); }

    /**
     * \brief Check is this is a subproperty
     * \details Property hierarchies may be created by making one or more statements that a property is a subproperty of one or more other properties. For example, hasSibling may be stated to be a subproperty of hasRelative. From this a reasoner can deduce that if an individual is related to another by the hasSibling property, then it is also related to the other by the hasRelative property.
     */
    bool subPropertyOf(const Name& name) { return subclassOf(name); }

    /**
     * \brief Set domain of property, i.e. the types it applies to
     * \details A domain of a property limits the individuals to which the property can be applied. If a property relates an individual to another individual, and the property has a class as one of its domains, then the individual must belong to the class. For example, the property hasChild may be stated to have the domain of Mammal. From this a reasoner can deduce that if Frank hasChild Anna, then Frank must be a Mammal. Note that rdfs:domain is called a global restriction since the
     * restriction is stated on the property and not just on the property when it is associated with a particular class. See the discussion below on property restrictions for more information.
     * \see http://www.w3.org/TR/2004/REC-owl-features-20040210/#term_domain
     */
    void setDomain(const Name& domain) { mDomain = domain; }

    /**
     * \brief Get domain of this property
     * \see setDomain
     */
    Name getDomain() { return mDomain; }

    /**
     * \brief Set range of this property
     * \see The range of a property limits the individuals that the property may have as its value. If a property relates an individual to another individual, and the property has a class as its range, then the other individual must belong to the range class. For example, the property hasChild may be stated to have the range of Mammal. From this a reasoner can deduce that if Louise is related to Deborah by the hasChild property, (i.e., Deborah is the child of Louise), then Deborah is a
     * Mammal. Range is also a global restriction as is domain above. Again, see the discussion below on local restrictions (e.g. AllValuesFrom) for more information
     * \see http://www.w3.org/TR/2004/REC-owl-features-20040210/#term_range
     * \see getRange
     */
    void setRange(const Name& range) { mRange = range; }

    /**
     * \brief Get range of this property
     * \see setRange
     */
    Name getRange() { return mRange; }

    /**
     * \brief Add inverse of this property
     * \see http://www.w3.org/TR/2004/REC-owl-features-20040210/#inverseOf
     * \details One property may be stated to be the inverse of another property. If the property P1 is stated to be the inverse of the property P2, then if X is related to Y by the P2 property, then Y is related to X by the P1 property. For example, if hasChild is the inverse of hasParent and Deborah hasParent Louise, then a reasoner can deduce that Louise hasChild Deborah.
     */
    void addInverseOf(const Name& name);

    /**
     * \brief Get list of inverse properties
     * \see addInverseOf
     * \see isInverseOf
     */
    NameList getInverseOf() const { return mInverseOf; }

    /**
     * \brief Test if this property is an inverse of any other property
     * \see addInverseOf
     * \see getInverseOf
     */
    bool isInverseOf() const { return !mInverseOf.empty(); }

    /**
     * \brief Test if this property is transitive
     * \details Properties may be stated to be transitive. If a property is transitive, then if the pair (x,y) is an instance of the transitive property P, and the pair (y,z) is an instance of P, then the pair (x,z) is also an instance of P. For example, if ancestor is stated to be transitive, and if Sara is an ancestor of Louise (i.e., (Sara,Louise) is an instance of the property ancestor) and Louise is an ancestor of Deborah (i.e., (Louise,Deborah) is an instance of the property
     *  ancestor), then a reasoner can deduce that Sara is an ancestor of Deborah (i.e., (Sara,Deborah) is an instance of the property ancestor).
     *  OWL Lite (and OWL DL) impose the side condition that transitive properties (and their superproperties) cannot have a maxCardinality 1 restriction. Without this side-condition, OWL Lite and OWL DL would become undecidable languages. See the property axiom section of the OWL Semantics and Abstract Syntax document for more information.
     * \see http://www.w3.org/TR/2004/REC-owl-features-20040210/#TransitiveProperty
     * \see setTransitive
     */
    bool isTransitive() const { return mTransitive; }

    /**
     * \brief Set this property to be transitive
     * \see isTransitive
     */
    void setTransitive() { mTransitive = true; }

    /**
     * \brief Test if this property is symmetric
     * \details Properties may be stated to be symmetric. If a property is symmetric, then if the pair (x,y) is an instance of the symmetric property P, then the pair (y,x) is also an instance of P. For example, friend may be stated to be a symmetric property. Then a reasoner that is given that Frank is a friend of Deborah can deduce that Deborah is a friend of Frank
     * \see setSymmetric
     * \see http://www.w3.org/TR/2004/REC-owl-features-20040210/#SymmetricProperty
     *
     */
    bool isSymmetric() const { return mSymmetric; }
    /**
     * \brief Set this property to be symmetric
     * \see isSymmetric
     */
    void setSymmetric() { mTransitive = true; }

    /**
     * \see http://www.w3.org/TR/2004/REC-owl-features-20040210/#FunctionalProperty
     * \details Properties may be stated to have a unique value. If a property is a FunctionalProperty, then it has no more than one value for each individual (it may have no values for an individual). This characteristic has been referred to as having a unique property. FunctionalProperty is shorthand for stating that the property's minimum cardinality is zero and its maximum cardinality is 1. For example, hasPrimaryEmployer may be stated to be a FunctionalProperty. From this a
     * reasoner may deduce that no individual may have more than one primary employer. This does not imply that every Person must have at least one primary employer however.
     */
    bool isFunctional() const { return mFunctional; }

    /**
     * \brief Set this property to be functional
     * \see isFunctional
     */
    void setFunctional() { mFunctional = true; }

    /**
     * \see http://www.w3.org/TR/2004/REC-owl-features-20040210/#InverseFunctionalProperty
     * \details Properties may be stated to be inverse functional. If a property is inverse functional then the inverse of the property is functional. Thus the inverse of the property has at most one value for each individual. This characteristic has also been referred to as an unambiguous property. For example, hasUSSocialSecurityNumber (a unique identifier for United States residents) may be stated to be inverse functional (or unambiguous). The inverse of this property (which may be
     * referred to as isTheSocialSecurityNumberFor) has at most one value for any individual in the class of social security numbers. Thus any one person's social security number is the only value for their isTheSocialSecurityNumberFor property. From this a reasoner can deduce that no two different individual instances of Person have the identical US Social Security Number. Also, a reasoner can deduce that if two instances of Person have the same social security number, then those
     * two instances refer to the same individual.
     */
    bool isInverseFunctional() const { return mInverseFunctional; }

    /**
     * \brief Set this property to be inverse functional
     * \see isInverseFunctional
     */
    void setInverseFunctional() { mInverseFunctional = true; }

    /**
     * \brief Convert property to string representation
     */
    std::string toString() const;

private:
    NameList mInverseOf;
    bool mSymmetric;
    bool mTransitive;
    bool mFunctional;
    bool mInverseFunctional;

    Name mDomain;
    Name mRange;
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
