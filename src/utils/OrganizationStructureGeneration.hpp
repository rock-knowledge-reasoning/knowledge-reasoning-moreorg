#ifndef ORGANIZATION_MODEL_UTILS_ORGANIZATION_STRUCTURE_GENERATION_HPP
#define ORGANIZATION_MODEL_UTILS_ORGANIZATION_STRUCTURE_GENERATION_HPP

#include <limits>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <numeric/IntegerPartitioning.hpp>
#include <base/Time.hpp>
#include <base-logging/Logging.hpp>
#include "../Agent.hpp"

namespace organization_model {
namespace utils {

typedef AtomicAgent::List Coalition;
typedef std::vector< Coalition > CoalitionStructure;

std::ostream& operator<<(std::ostream& os, const Coalition& coalition);
std::ostream& operator<<(std::ostream& os, const CoalitionStructure& list);

struct Bounds
{
    Bounds(double exact);
    Bounds();

    double maximum;
    double average;
    double minimum;

    std::string toString() const;
};


std::ostream& operator<<(std::ostream& os, const Bounds& bounds);

/**
 * This is an adapted implementation of the coalition structure generation as described in:
 * "An Anytime Algorithm for Optimal Coalition Structure Generation", (Rahwan et al., 2009)
 * Please note that the current implementation uses recursion
 *
 * The adaption allows to account for a use of model pools and looking at the
 * identification coalition functions that have a boolean as characteristic value (1.0 or 0.0)
 * and thus represent an activation
 */
class CoalitionStructureGeneration
{
public:
    typedef boost::function1<double, const Coalition&> CoalitionValueFunction;
    typedef boost::function1<double, const CoalitionStructure&> CoalitionStructureValueFunction;

    struct Statistics
    {
        std::vector<numeric::IntegerPartition> allIntegerPartitions;
        std::vector<numeric::IntegerPartition> prunedIntegerPartitions;
        std::vector<numeric::IntegerPartition> searchedIntegerPartitions;

        std::vector<numeric::IntegerPartition> remainingIntegerPartitions() const;

        std::string toString() const;
    };

private:
    AtomicAgent::List mAgents;
    Statistics mStatistics;

    // Agents listed by size of the coalition
    typedef std::map<size_t, std::set< Coalition > > AgentCoalitionMap;
    AgentCoalitionMap mAgentCoalitionMap;

    typedef std::map<size_t, Bounds> CoalitionBoundMap;
    CoalitionBoundMap mCoalitionBoundMap;

    CoalitionValueFunction mCoalitionValueFunction;
    CoalitionStructureValueFunction mCoalitionStructureValueFunction;

    typedef std::map<numeric::IntegerPartition, Bounds> IntegerPartitionBoundsMap;
    IntegerPartitionBoundsMap mIntegerPartitionBoundsMap;

    mutable boost::mutex mSolutionMutex;
    mutable boost::mutex mStatisticsMutex;
    boost::thread mThread;
    base::Time mStartTime;
    base::Time mCompletionTime;
    CoalitionStructure mCurrentBestCoalitionStructure;
    double mCurrentBestCoalitionStructureValue;
    double mCurrentSolutionQuality;
    double mGlobalUpperBound;

    /**
     * Compute the integer partitions and the agent coalition map for coalition size up to
     * the maximum number of agents
     */
    void prepare();

    double bestLowerBound(const CoalitionBoundMap& boundMap);
    double bestLowerBound(const IntegerPartitionBoundsMap& boundMap);
    double bestUpperBound(const IntegerPartitionBoundsMap& boundMap);

    Bounds computeIntegerPartitionBounds(const numeric::IntegerPartition& partition);
    IntegerPartitionBoundsMap prune(const IntegerPartitionBoundsMap& boundsMap);
    CoalitionBoundMap prune(const CoalitionBoundMap& boundMap);

    numeric::IntegerPartition selectIntegerPartition(const IntegerPartitionBoundsMap& boundMap, double maximumBound) const;

    /**
     * Search a subspace
     * \param partition The integer partition to be used
     * \param k index of the entry to start partial coalition with (mainly required for recursive use), e.g., of the integer partition [2,1,1]
     * \param alpha
     * \param agents list of agents that have to be considered for appending to
     * the existing coalition structure
     * \param currentStructure the already constructed coalition structure
     * \param bestStar Quality of the solution, i.e. 1.05 means 95% percent of the optimal solution
     * \return true if this subspace contained a better solution than already existed
     */
    bool searchSubspace(const numeric::IntegerPartition& partition, size_t k, size_t alpha, const AtomicAgent::List& agents, const CoalitionStructure& currentStructure, double betaStar);

    bool updateCurrentBestCoalitionStructure(const CoalitionStructure& coalitionStructure, double value);

public:
    /**
     * Find best coalitionstructure -- will block until coalition structure is found
     */
    CoalitionStructure findBest(double quality = 1.0);

    /**
     * Reset in order to restart a new search
     */
    void reset();

    /**
     * Search for a solution and allow retrieval of intermediate results via currentBestSolution
     */
    void anytimeSearch(double quality = 1.0);

    /**
     * Check if anytimeSearch completed
     * \return true, upon completition, false otherwise -- if search has not been started at all, it will return false
     */
    bool anytimeSearchCompleted() const { return mCompletionTime != base::Time(); }

    /**
     * Compute total elapsed time since last search start
     * \return time object representing the elapsed time, i.e. use toSeconds() or corresponding function to retrieve detailled value
     */
    base::Time elapsed() const { return base::Time::now() - mStartTime; }

    /**
     * Stop the search before completion
     */
    void stopSearch();

    /**
     * Retrieve the current best solution
     * return the current best solution, if none has been found it will return an empty CoalitionStructure
     */
    CoalitionStructure currentBestSolution() const;

    double currentBestSolutionValue() const;
    double currentBestSolutionQuality() const;


    /**
     * \params agents List of agents that are available
     * \param coalitionValueFunction Function that allows to compute the value of an individual coalition
     * \param coalitionStructureValueFunction Function that allows to compute the value of a coalition structure
     */
    CoalitionStructureGeneration(const AtomicAgent::List& agents,
            CoalitionValueFunction coalitionValueFunction,
            CoalitionStructureValueFunction coalitionStructureValueFunction);


    /**
     * Stringify status of this CoalitionStructureGeneration object
     * \return stringified instance
     */
    std::string toString() const;

    /**
     * Stringify a CoalitionStructure
     * \return stringified instance of CoalitionStructure
     */
    static std::string toString(const CoalitionStructure& c);

    Statistics getStatistics() const;
};

} // end namespace utils
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_COALITION_STRUCTURE_GENERATION_HPP
