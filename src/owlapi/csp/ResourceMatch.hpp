#ifndef OWLAPI_CSP_RESOURCE_MATCH_HPP
#define OWLAPI_CSP_RESOURCE_MATCH_HPP

#include <gecode/set.hh>
#include <gecode/int.hh>
#include <gecode/search.hh>

#include <owl_om/owlapi/model/OWLOntology.hpp>
#include <owl_om/owlapi/model/OWLOntologyAsk.hpp>
#include <base/Logging.hpp>

namespace owlapi {
namespace csp {

typedef std::map<owlapi::model::IRI, std::vector<int> > TypeInstanceMap;
typedef std::map<owlapi::model::IRI, owlapi::model::IRIList > AllowedTypesMap;
typedef owlapi::model::IRIList InstanceList;
typedef owlapi::model::IRIList TypeList;


class ResourceMatch : public Gecode::Space
{
    std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> mQueryRestrictions;
    std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> mResourcePoolRestrictions;
    std::map<owlapi::model::OWLCardinalityRestriction::Ptr, InstanceList> mSolution;
    InstanceList mResourcePool;

    /**
     * Assignments of query resources to pool resources. This is what has to be solved.
     */
    Gecode::SetVarArray mSetAssignment;

    ResourceMatch* solve();

protected:
    ResourceMatch(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& queryRestrictions, const InstanceList& resourcePoolRestrictions, owlapi::model::OWLOntology::Ptr ontology);

    /**
     * Search support
     * This copy constructor is required for the search engine
     * and it has to provide a deep copy
     */
    ResourceMatch(bool share, ResourceMatch& s);

    /**
     * This method is called by the search engine
     */
    virtual Gecode::Space* copy(bool share);

    static TypeInstanceMap toTypeInstanceMap(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& restrictions);
    static TypeList getTypeList(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& restrictions);

    static AllowedTypesMap getAllowedTypes(const TypeInstanceMap& query, const TypeInstanceMap& pool, owlapi::model::OWLOntology::Ptr ontology);

    static std::vector<int> getAllowedDomain(const owlapi::model::IRI& qualificationItem, const AllowedTypesMap& allowedTypes, const TypeInstanceMap& typeInstanceMap);

    static uint32_t getInstanceCount(const TypeInstanceMap& map);

    /**
     * Map solution from internally used integer list to input format, i.e.
     * cardinality restrictions
     */
    void remapSolution();

public:
    void print(std::ostream& os) const;

    /**
     * Convert restriction list to a list of instance, e.g. min 4 Item will
     * converted to a list of size 4 Items (qualification of the restriction)
     * \return List of (model) instances
     */
    static InstanceList getInstanceList(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& restrictions);

    /*
     * constrain function for best solution search. the
     * currently best solution _b is passed and we have to constraint that this solution can only
     * be better than b, for it to be excluded if it isn't
     */
    //virtual void constrain(const Gecode::Space& _b);

    /**
     * Construct a solution with an initial situation to search
     * \throw std::runtime_error if a solution could not be found
     */
    static ResourceMatch* solve(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& queryRestrictions, const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& resourcePoolRestrictions, owlapi::model::OWLOntology::Ptr ontology);

    /**
     * Solve the given constraint satisfaction problem
     * \param queryRestrictions The restrictions to be fulfilled
     * \param resourePool The items available to fulfill the restriction
     * \param ontology Ontology to check whether an item in the resource pool is a valid replacement for an item in the query
     */
    static ResourceMatch* solve(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& queryRestrictions, const InstanceList& resourcePool, owlapi::model::OWLOntology::Ptr ontology);

    /**
     * Create a string representation of this object
     * \return String
     */
    std::string toString() const;

    /**
     * Retrieve assignment of a solution for a given restriction
     * \return InstanceList that has been assigned from the resourcePool to
     * fulfill this restriction
     */
    InstanceList getAssignedResources(owlapi::model::OWLCardinalityRestriction::Ptr restriction) const;

    /**
     * Retrieve the list of resources that remain unassigned
     */
    InstanceList getUnassignedResources() const;
};

} // end namespace cps
} // end namespace owlapi
#endif // OWLAPI_CSP_RESOURCE_MATCH_HPP
