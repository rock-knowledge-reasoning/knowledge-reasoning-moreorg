#ifndef ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP
#define ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP

#include <gecode/set.hh>
#include <gecode/search.hh>
#include <base/Time.hpp>
#include <functional>
#include <unordered_map>
#include <tuple>

#include <numeric/Stats.hpp>
#include <graph_analysis/BaseGraph.hpp>
#include "../OrganizationModelAsk.hpp"
#include "../vocabularies/OM.hpp"
#include <qxcfg/Configuration.hpp>

namespace organization_model {
namespace algebra {

typedef std::tuple<ModelPool, owlapi::model::IRI, owlapi::model::IRI, double, size_t>
    FeasibilityQuery;

typedef std::unordered_map<FeasibilityQuery,
        std::pair<graph_analysis::BaseGraph::Ptr, bool> > QueryCache;
} // end namespace algebra
} // end namespace organization_model


namespace std {
using namespace owlapi::model;
using namespace organization_model::algebra;
template<>
struct hash< FeasibilityQuery >
{
    size_t operator()(const FeasibilityQuery& query) const
    {
        size_t seed = 0;
        for(const ModelPool::value_type& v : get<0>(query))
        {
            boost::hash_combine(seed, v.first.toString());
            boost::hash_combine(seed, v.second);
        }
        boost::hash_combine(seed, get<1>(query).toString());
        boost::hash_combine(seed, get<2>(query).toString());
        boost::hash_combine(seed, get<3>(query));
        boost::hash_combine(seed, get<4>(query));
        return seed;
    }
};
} // end namespace std

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

    // Explicitly enumerated type (in contrast to the cardinality based
    // representation via ModelPool
    ModelCombination mModelCombination;
    owlapi::model::IRIList mInterfaces;

    // Index of interface mapping and interface index range correspond to the
    // same model instance
    //
    /// List the interfaces and associate the list with corresponding model instance
    /// as such allows to map an agent (as model instance) to the list of
    //interfaces
    std::vector< std::pair<owlapi::model::IRI, owlapi::model::IRIList> > mInterfaceMapping;
    /// Register the interface index ranges
    typedef std::pair<uint32_t, uint32_t> IndexRange;
    // Per model instance, list the range of interfaces that are associated with
    // this model to speed up information access
    std::vector< IndexRange > mInterfaceIndexRanges;

    /// Map from the index of a (connection) variable to the two agents/index of index ranges
    std::vector< std::pair<size_t, size_t> > mIdx2Agents;

    // |#ofInterface|*a0Idx + a1Idx
    Gecode::IntVarArray mConnections;
    Gecode::IntVarArray mExistingConnections;

    // Helper class to improve computation of merit function
    Gecode::IntVarArray mAgentConnections;

    // Random number generator
    mutable Gecode::Rnd mRnd;

    mutable graph_analysis::BaseGraph::Ptr mpBaseGraph;

    /// Make sure the connected system forms a tree
    bool mIsTree;

    void setTreeRequirement(bool isTree) { mIsTree = isTree; }

    Gecode::Symmetries identifySymmetries(Gecode::IntVarArray& connections);

    bool isComplete() const;

    /**
     * Populate the
     * InterfaceIndexRange and InterfaceMapping to allow identification of
     * interfaces which belong to an atomic agent (model instance)
     */
    void identifyInterfaces();
    void enforceSymmetricMatrix(Gecode::IntVarArray& connections);
    void applyCompatibilityConstraints(Gecode::IntVarArray& connections);
    void cacheExistingConnections(Gecode::IntVarArray& connections);
    void maxOneLink(Gecode::IntVarArray& connections);

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
     * \param minFeasible Minimum number of feasible combinations that need to
     * be found
     * \param interfaceBaseClass The base type for the interfaces that have to
     * be considered
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

    /**
     * Retrieve the connection graph of the last feasibility check
     * \return connection graph
     */
    static const graph_analysis::BaseGraph::Ptr& getConnectionGraph() { return msConnectionGraph; }

protected:
    static Connectivity::Statistics msStatistics;
    static graph_analysis::BaseGraph::Ptr msConnectionGraph;

    // General configuration to control, e.g. the branching behaviour
    static qxcfg::Configuration msConfiguration;

    class NoConnectionInterfaces : public std::runtime_error
    {
        public:
            NoConnectionInterfaces(const std::string& message)
                : std::runtime_error(message)
            {}
    };

    static QueryCache mQueryCache;
};


} // end namespace algebra
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_CONNECTIVITY_HPP
