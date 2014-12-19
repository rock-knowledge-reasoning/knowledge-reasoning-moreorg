#ifndef OWLAPI_CSP_RESOURCE_MATCH_HPP
#define OWLAPI_CSP_RESOURCE_MATCH_HPP

#include <gecode/int.hh>
#include <gecode/search.hh>

#include <owl_om/owlapi/model/OWLOntology.hpp>
#include <owl_om/owlapi/model/OWLOntologyAsk.hpp>
#include <base/Logging.hpp>

namespace owlapi {
namespace csp {


class ResourceMatch : public Gecode::Space
{
    owlapi::model::IRIList mQuery;
    owlapi::model::IRIList mResourcePool;
    /**
     * Assignments of query resources to pool resources. This is what has to be solved.
     */
    Gecode::IntVarArray mAssignment;

    owlapi::model::OWLOntologyAsk mAsk;

public:

    ResourceMatch(owlapi::model::IRIList query, owlapi::model::IRIList resourcePool, owlapi::model::OWLOntology::Ptr ontology);

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

    ///*
    // * constrain function for best solution search. the
    // * currently best solution _b is passed and we have to constraint that this solution can only
    // * be better than b, for it to be excluded if it isn't
    // */
    //virtual void constrain(const Gecode::Space& _b)

    /**
     * Construct a solution with an initial situation to search
     * \throw std::runtime_error if a solution could not be found
     */
    static ResourceMatch* solve(owlapi::model::IRIList query, owlapi::model::IRIList resourcePool, owlapi::model::OWLOntology::Ptr ontology);

    /**
     * Create a string representation of this object
     * \return String
     */
    std::string toString() const;

    /**
     * Retrieve assignment of a solution
     * \return IRI that has been assigned from the resourcePool
     */
    owlapi::model::IRI getAssignment(const owlapi::model::IRI& iri) const;
};

} // end namespace cps
} // end namespace owlapi
#endif // OWLAPI_CSP_RESOURCE_MATCH_HPP
