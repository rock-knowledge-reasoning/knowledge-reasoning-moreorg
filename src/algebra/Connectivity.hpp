#ifndef ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP
#define ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP

#include <gecode/set.hh>
#include <gecode/search.hh>
#include <base/Time.hpp>
#include <numeric/Stats.hpp>
#include <graph_analysis/BaseGraph.hpp>
#include "../OrganizationModelAsk.hpp"
#include "../vocabularies/OM.hpp"
#include <qxcfg/Configuration.hpp>

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
    owlapi::model::IRI mProperty;

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

    /// Map from the index of a variable to the two agents/index of index ranges
    std::vector< std::pair<size_t, size_t> > mIdx2Agents;

    // |#ofInterface|*a0Idx + a1Idx
    Gecode::IntVarArray mConnections;

    // Helper class to improve computation of merit function
    Gecode::IntVarArray mAgentConnections;

    // Random number generator
    mutable Gecode::Rnd mRnd;

    mutable graph_analysis::BaseGraph::Ptr mpBaseGraph;

    Gecode::Symmetries identifySymmetries(Gecode::IntVarArray& connections);

    //static void doPostCheckGraphCompleteness(Gecode::Space& home);
    //void checkGraphCompleteness();
    bool isComplete() const;

public:

    struct Statistics
    {
        Statistics();

        uint64_t evaluations;
        double timeInS;
        int stopped;
        /**
         * Statistics of the underlying csp search:
         *     fail: number of failed nodes in search tree
         *     node: number of nodes expanded
         *     depth: maximum depth of search tree
         *     restart: number of restarts
         *     nogood: number of nogoods posted
         * \see http://www.gecode.org/doc-latest/reference/classGecode_1_1Search_1_1Statistics.html
         */
        Gecode::Search::Statistics csp;

        std::string toString(size_t indent = 0) const;


        /**
         * Get a description of the field of the statistics vector
         */
        static std::string getStatsDescription();

        /**
         * Compute the combined stats
         * \see getStatsDescription
         */
        static std::vector<numeric::Stats<double> > compute(const std::vector<Connectivity::Statistics>& stats);

        static std::string toString(const std::vector<Connectivity::Statistics>& stats);
    };

    Connectivity(const ModelPool& modelPool,
            const OrganizationModelAsk& ask,
            const owlapi::model::IRI& interfaceBaseClass = vocabulary::OM::resolve("ElectroMechanicalInterface"),
            const owlapi::model::IRI& property = vocabulary::OM::has()
    );

    /**
     * Search support
     * This copy constructor is required for the search engine
     * and it has to provide a deep copy
     */
    Connectivity(Connectivity& s);

    virtual ~Connectivity() {};

    static void setConfiguration(const qxcfg::Configuration& configuration) { msConfiguration = configuration; }

    qxcfg::Configuration& getConfiguration() { return msConfiguration; }

    /**
     * Create a copy of this space
     * This method is called by the search engine
     */
    virtual Gecode::Space* copy(void);

    /**
     * Check whether a model pool can be fully connected
     * \param modelPool ModelPool to check if all agents can form a single unit
     * \param ask OrganizationModel to use for information about available
     * interfaces etc.
     * \param timeoutInMs Timeout of the feasibility check, default is 0
     * \return True if a connection is feasible, false otherwise
     */
    static bool isFeasible(const ModelPool& modelPool, const OrganizationModelAsk& ask, double timeoutInMs = 0, size_t minFeasible = 1,
            const owlapi::model::IRI& interfaceBaseClass = vocabulary::OM::resolve("ElectroMechanicalInterface") );

    /**
     * Check whether a model pool can be fully connected
     * \param modelPool ModelPool to check if all agents can form a single unit
     * \param ask OrganizationModel to use for information about available
     * interfaces etc.
     * \param timeoutInMs Timeout of the feasibility check, default is 0
     * \param baseGraph that hold the resulting connection graph
     * \return True if a connection is feasible, false otherwise
     */
    static bool isFeasible(const ModelPool& modelPool, const OrganizationModelAsk& ask, graph_analysis::BaseGraph::Ptr& baseGraph, double timeoutInMs = 0, size_t minFeasible = 1,
            const owlapi::model::IRI& interfaceBaseClass = vocabulary::OM::resolve("ElectroMechanicalInterface") );

    /**
     * Convert solution to string
     */
    std::string toString() const;

    static double merit(const Gecode::Space& space, Gecode::IntVar x, int idx);

    /**
     * Define the value selection merit function
     */
    double computeMerit(Gecode::IntVar x, int idx) const;

    /**
     * Return the number of evaluations for the last feasibility check
     */
    static const Connectivity::Statistics& getStatistics() { return msStatistics; }

protected:
    static Connectivity::Statistics msStatistics;

    // General configuration to control, e.g. the branching behaviour
    static qxcfg::Configuration msConfiguration;

};


} // end namespace algebra
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP
