#ifndef ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP
#define ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP

#include <gecode/set.hh>
#include <gecode/search.hh>
#include <organization_model/OrganizationModelAsk.hpp>
#include <organization_model/vocabularies/OM.hpp>

namespace organization_model {
namespace algebra {

/**
 * \class Connectivity
 * \brief Allow to check the feasibility of interfacing, i.e., whether or not
 * there is a feasible connection for a set of agents
 */
class Connectivity : public Gecode::Space
{
    /// Model pool which has to be checked for its connectivity
    ModelPool mModelPool;
    /// The organization model
    owlapi::model::OWLOntologyAsk mAsk;

    owlapi::model::IRI mInterfaceBaseClass;

    ModelCombination mModelCombination;
    owlapi::model::IRIList mInterfaces;

    // Index of interface mapping and interface index range correspond to the
    // same model instance
    //
    /// List the interfaces and associate the list with corresponding model instance
    std::vector< std::pair<owlapi::model::IRI, owlapi::model::IRIList> > mInterfaceMapping;
    /// Register the interface index ranges
    typedef std::pair<uint32_t, uint32_t> IndexRange;
    std::vector< IndexRange > mInterfaceIndexRanges;
    Gecode::IntVarArray mConnections;

    Gecode::Symmetries identifySymmetries(Gecode::IntVarArray& connections);

public:
    Connectivity(const ModelPool& modelPool, const OrganizationModelAsk& ask, const owlapi::model::IRI& interfaceBaseClass = vocabulary::OM::resolve("ElectroMechanicalInterface") );

    /**
     * Search support
     * This copy constructor is required for the search engine
     * and it has to provide a deep copy
     */
    Connectivity(bool share, Connectivity& s);

    virtual ~Connectivity() {};

    /**
     * Create a copy of this space
     * This method is called by the search engine
     */
    virtual Gecode::Space* copy(bool share);

    /**
     * Check whether a model pool can be fully connected
     * \return True if a connection is feasible, false otherwise
     */
    static bool isFeasible(const ModelPool& modelPool, const OrganizationModelAsk& ask);

    /**
     * Convert solution to string
     */
    std::string toString() const;

};

} // end namespace algebra
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP
