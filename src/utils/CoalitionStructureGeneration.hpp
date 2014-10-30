#ifndef MULTIAGENT_UTILS_COALITION_STRUCTURE_GENERATION_HPP
#define MULTIAGENT_UTILS_COALITION_STRUCTURE_GENERATION_HPP

#include <limits>
#include <owl_om/utils/IntegerPartitioning.hpp>
#include <owl_om/Combinatorics.hpp>
#include <boost/function.hpp>
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

    typedef std::map<IntegerPartition, Bounds> IntegerPartitionBoundsMap;
    IntegerPartitionBoundsMap mIntegerPartitionBoundsMap;


    /**
     * Compute the integer partitions and the agent coalition map for coalition size up to
     * the maximum number of agents
     */
    void prepare();

    double bestLowerBound(const CoalitionBoundMap& boundMap);
    double bestLowerBound(const IntegerPartitionBoundsMap& boundMap);
    double bestUpperBound(const IntegerPartitionBoundsMap& boundMap);

    Bounds computeIntegerPartitionBounds(const IntegerPartition& partition);
    IntegerPartitionBoundsMap prune(const IntegerPartitionBoundsMap& boundsMap);
    CoalitionBoundMap prune(const CoalitionBoundMap& boundMap);

    IntegerPartition selectIntegerPartition(const IntegerPartitionBoundsMap& boundMap, double maximumBound) const;

    /**
     *
     * \param globalUpperBound
     * \param bestStar Quality of the solution, i.e. 1.05 means 95% percent of the optimal solution
     */
    CoalitionStructure searchSubspace(const IntegerPartition& partition, size_t k, size_t alpha, const AgentList& agents, CoalitionStructure bestStructure, double bestStructureValue, const CoalitionStructure& currentStructure, double globalUpperBound, double betaStar);

public:

    /**
     * \params agents List of agents that are available
     * \param coalitionValueFunction Function that allows to compute the value of an individual coalition
     * \param coalitionStructureValueFunction Function that allows to compute the value of a coalition structure
     */
    CoalitionStructureGeneration(const AgentList& agents, CoalitionValueFunction coalitionValueFunction, CoalitionStructureValueFunction coalitionStructureValueFunction);

    CoalitionStructure findBest(double quality = 1.0);

    std::string toString() const;
};

} // end namespace utils
} // end namespace multiagent
#endif // MULTIAGENT_UTILS_COALITION_STRUCTURE_GENERATION_HPP
