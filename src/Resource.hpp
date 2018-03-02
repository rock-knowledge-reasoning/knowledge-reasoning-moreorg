#ifndef ORGANIZATION_MODEL_RESOURCE_HPP
#define ORGANIZATION_MODEL_RESOURCE_HPP

#include <map>
#include <set>
#include "PropertyConstraint.hpp"

namespace organization_model {

/**
 * TODO: consider FunctionalitySet -> PropertyConstraint as Edge and HyperEdge
 */
class Resource
{
public:
    typedef std::vector<Resource> List;
    typedef std::set<Resource> Set;

    Resource(const owlapi::model::IRI& model = owlapi::model::IRI());

    virtual ~Resource();

    Resource(const owlapi::model::IRI& model,
            const PropertyConstraint::List& propertyConstraints);

    Resource(const owlapi::model::IRI& model,
            const PropertyConstraint::Set& propertyConstraints);

    bool operator<(const Resource& other) const;

    const owlapi::model::IRI& getModel() const { return mModel; }

    /**
     * Convert list of models to resource set
     */
    static Set toResourceSet(const owlapi::model::IRIList& model);


    void setPropertyConstraints(const PropertyConstraint::Set& constraints) { mPropertyConstraints = constraints; }
    const PropertyConstraint::Set& getPropertyConstraints() const { return mPropertyConstraints; }

    void addPropertyConstraint(const PropertyConstraint& constraint);

    /**
     * Add a list of property constraints
     */
    void addPropertyConstraints(const PropertyConstraint::List& constraints);
    void addPropertyConstraints(const PropertyConstraint::Set& constraints);

    std::string toString(size_t indent = 0) const;

    static std::string toString(const Resource::List& requirements, size_t indent = 0);
    static std::string toString(const Resource::Set& requirements, size_t indent = 0);

private:
    owlapi::model::IRI mModel;
    PropertyConstraint::Set mPropertyConstraints;
};

typedef Resource Functionality;

} // namespace organization_model
#endif // ORGANIZATION_MODEL_RESOURCE_HPP
