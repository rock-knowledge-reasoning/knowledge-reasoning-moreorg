#ifndef ORGANIZATION_MODEL_RESOURCE_HPP
#define ORGANIZATION_MODEL_RESOURCE_HPP

#include "PropertyConstraint.hpp"
#include <map>
#include <set>

namespace moreorg {

/**
 * TODO: consider FunctionalitySet -> PropertyConstraint as Edge and HyperEdge
 */
class Resource
{
public:
    using List = std::vector<Resource>;
    using Set = std::set<Resource>;

    Resource(const owlapi::model::IRI& model = owlapi::model::IRI());

    virtual ~Resource();

    Resource(const owlapi::model::IRI& model,
             const PropertyConstraint::List& propertyConstraints);

    Resource(const owlapi::model::IRI& model,
             const PropertyConstraint::Set& propertyConstraints);

    bool operator<(const Resource& other) const;
    bool operator==(const Resource& other) const;

    const owlapi::model::IRI& getModel() const { return mModel; }

    /**
     * Convert list of models to resource set
     */
    static Set toResourceSet(const owlapi::model::IRIList& model);

    /**
     * Get the list of models
     */
    static owlapi::model::IRISet getModels(const Resource::Set& a);

    void setPropertyConstraints(const PropertyConstraint::Set& constraints)
    {
        mPropertyConstraints = constraints;
    }
    const PropertyConstraint::Set& getPropertyConstraints() const
    {
        return mPropertyConstraints;
    }

    void addPropertyConstraint(const PropertyConstraint& constraint);

    /**
     * Add a list of property constraints
     */
    void addPropertyConstraints(const PropertyConstraint::List& constraints);
    void addPropertyConstraints(const PropertyConstraint::Set& constraints);

    std::string toString(size_t indent = 0) const;

    static std::string toString(const Resource::List& requirements,
                                size_t indent = 0);
    static std::string toString(const Resource::Set& requirements,
                                size_t indent = 0);

    void merge(const Resource& other);
    static Resource merge(const Resource& a, const Resource& b);
    static Resource::Set merge(const Resource::Set& resources);
    static Resource::Set merge(const Resource::Set& a, const Resource::Set& b);

private:
    owlapi::model::IRI mModel;
    PropertyConstraint::Set mPropertyConstraints;
};

using Functionality = Resource;

} // namespace moreorg
#endif // ORGANIZATION_MODEL_RESOURCE_HPP
