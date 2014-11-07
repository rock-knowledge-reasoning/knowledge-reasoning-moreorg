#ifndef MULTIAGENT_UTILS_COALITION_STRUCTURE_GENERATION_HPP
#define MULTIAGENT_UTILS_COALITION_STRUCTURE_GENERATION_HPP

#include <limits>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <numeric/IntegerPartitioning.hpp>
#include <base/Time.hpp>
#include <base/Logging.hpp>

namespace multiagent {
namespace utils {

typedef std::string Agent;
typedef std::vector<Agent> AgentList;
typedef AgentList Coalition;
typedef std::vector< Coalition > CoalitionStructure;

std::ostream& operator<<(std::ostream& os, const AgentList& list);
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
 * This is an implementation of the coalition structure generation as described in:
 * "An Anytime Algorithm for Optimal Coalition Structure Generation", (Rahwan et al., 2009)
 *
 * Please not that the current implementation uses recursion, but that will be subject to change.
 */
class CoalitionStructureGeneration
{
public:
    typedef boost::function1<double, const Coalition&> CoalitionValueFunction;
    typedef boost::function1<double, const CoalitionStructure&> CoalitionStructureValueFunction;

private:
    AgentList mAgents;

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
     *
     * \param globalUpperBound
     * \param bestStar Quality of the solution, i.e. 1.05 means 95% percent of the optimal solution
     * \return true if this subspace contained a better solution than already existed
     */
    bool searchSubspace(const numeric::IntegerPartition& partition, size_t k, size_t alpha, const AgentList& agents, const CoalitionStructure& currentStructure, double betaStar);

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
    CoalitionStructureGeneration(const AgentList& agents, CoalitionValueFunction coalitionValueFunction, CoalitionStructureValueFunction coalitionStructureValueFunction);


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
};

} // end namespace utils
} // end namespace multiagent
#endif // MULTIAGENT_UTILS_COALITION_STRUCTURE_GENERATION_HPP
