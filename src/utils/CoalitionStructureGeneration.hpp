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
typedef std::set< Coalition > CoalitionStructure;

struct Bounds
{
    Bounds()
        : maximum(std::numeric_limits<double>::min())
        , average(0.0)
        , minimum(std::numeric_limits<double>::max())
    {}

    double maximum;
    double average;
    double minimum;

    std::string toString() const
    {
        std::stringstream ss;
        ss.precision(10);
        ss << "Bounds: max[" << maximum << "] min[" << minimum << "] average[" << average << "]";
        return ss.str();
    }
};

std::ostream& operator<<(std::ostream& os, const Bounds& bounds)
{
    os << bounds.toString();
    return os;
}

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

public:

    /**
     * \params agents List of agents that are available
     * \param coalitionValueFunction Function that allows to compute the value of an individual coalition
     * \param coalitionStructureValueFunction Function that allows to compute the value of a coalition structure
     */
    CoalitionStructureGeneration(const AgentList& agents, CoalitionValueFunction coalitionValueFunction, CoalitionStructureValueFunction coalitionStructureValueFunction)
        : mAgents(agents)
        , mCoalitionValueFunction(coalitionValueFunction)
        , mCoalitionStructureValueFunction(coalitionStructureValueFunction)
    {
    }

    /**
     * Compute the integer partitions and the agent coalition map for coalition size up to 
     * the maximum number of agents
     */
    void prepare()
    {
        using namespace base::combinatorics;
        Combination<std::string> combinations(mAgents, mAgents.size(), MAX);
        do {

            std::vector<std::string> coalition = combinations.current();
            mAgentCoalitionMap[coalition.size()].insert(coalition);
        } while(combinations.next());


        // Compute bounds for coalitions a given size
        AgentCoalitionMap::const_iterator ait = mAgentCoalitionMap.begin();
        for(; ait != mAgentCoalitionMap.end(); ++ait)
        {
            Bounds bounds;

            const std::set< Coalition >& coalitions = ait->second;

            std::set<Coalition>::const_iterator cit = coalitions.begin();
            double sum = 0.0;
            for(;  cit != coalitions.end(); ++cit)
            {
                const Coalition& coalition = *cit;
                double value = mCoalitionValueFunction(coalition);

                LOG_DEBUG_S << "Coalition value: " << value;

                sum += value;
                if(value > bounds.maximum)
                {
                    bounds.maximum = value;
                }
                if(value < bounds.minimum)
                {
                    bounds.minimum = value;
                }
            }
            bounds.average = sum / coalitions.size();

            size_t coalitionSize = ait->first;
            mCoalitionBoundMap[coalitionSize] = bounds;
        }

        // Compute integer partition, e.g. for 4 agents
        //     [4]
        // [3,1][2,2]
        //   [2,1,1]
        //  [1,1,1,1]
        IntegerPartitioning ip;
        ip.compute(mAgents.size());
        IntegerPartitioning::PartitionsMap partitionsMap = ip.getPartitionsMap();

        // Compute bounds for integer partitions
        IntegerPartitioning::PartitionsMap::const_iterator cit = partitionsMap.begin();
        for(; cit != partitionsMap.end(); ++cit)
        {
            const std::set<IntegerPartition>& partitions = cit->second;
            std::set<IntegerPartition>::const_iterator pit = partitions.begin();
            for(; pit != partitions.end(); ++pit)
            {
                Bounds bounds = computeIntegerPartitionBounds(*pit);
                mIntegerPartitionBoundsMap[*pit] = bounds;
            }
        }

        IntegerPartitionBoundsMap updatedIntegerPartitionsBoundMap = prune(mIntegerPartitionBoundsMap);
    }

    double bestLowerBound(const CoalitionBoundMap& boundMap)
    {
        double lowerBound = std::numeric_limits<double>::min();
        CoalitionBoundMap::const_iterator cit = boundMap.begin();
        for(; cit != boundMap.end(); ++cit)
        {
            Bounds bounds = cit->second;
            if(bounds.average > lowerBound)
            {
                lowerBound = bounds.average;
            }
        }
        return lowerBound;
    }

    double bestLowerBound(const IntegerPartitionBoundsMap& boundMap)
    {
        double lowerBound = std::numeric_limits<double>::min();
        IntegerPartitionBoundsMap::const_iterator cit = boundMap.begin();
        for(; cit != boundMap.end(); ++cit)
        {
            Bounds bounds = cit->second;
            if(bounds.average > lowerBound)
            {
                lowerBound = bounds.average;
            }
        }
        return lowerBound;
    }

    Bounds computeIntegerPartitionBounds(const IntegerPartition& partition)
    {
        IntegerPartition::const_iterator cit = partition.begin(); 
        Bounds bounds;
        bounds.maximum = 0;
        bounds.average = 0;
        bounds.minimum = 0;
        for(; cit != partition.end(); ++cit)
        {
            int coalitionSize = *cit;
            Bounds coalitionBound = mCoalitionBoundMap[coalitionSize];

            bounds.maximum += coalitionBound.maximum;
            bounds.average += coalitionBound.average;
            bounds.minimum += coalitionBound.minimum;
        }

        return bounds;
    }

    IntegerPartitionBoundsMap prune(const IntegerPartitionBoundsMap& boundsMap)
    {
        double globalLowerBound = bestLowerBound(boundsMap);

        IntegerPartitionBoundsMap updatedBoundsMap = boundsMap;
        IntegerPartitionBoundsMap::const_iterator cit = boundsMap.begin();

        for(; cit != boundsMap.end(); ++cit)
        {
            Bounds bounds = cit->second;
            if(bounds.maximum < globalLowerBound)
            {
                LOG_DEBUG_S << "Removing partition: " << cit->first << " -- max: " << bounds.maximum << " < global min: " << globalLowerBound;
                updatedBoundsMap.erase(cit->first);
            }
        }
        return updatedBoundsMap;
    }


    CoalitionBoundMap prune(const CoalitionBoundMap& boundMap)
    {
        double globalLowerBound = bestLowerBound(boundMap);

        CoalitionBoundMap updatedCoalitionBoundMap = boundMap;

        CoalitionBoundMap::const_iterator cit = boundMap.begin();
        for(; cit != boundMap.end(); ++cit)
        {
            Bounds bounds = cit->second;
            if(bounds.maximum < globalLowerBound)
            {
                LOG_DEBUG_S << "Removing partition: " << cit->first << " -- max: " << bounds.maximum << " < global min: " << globalLowerBound;
                updatedCoalitionBoundMap.erase(cit->first);
            }
        }
        return updatedCoalitionBoundMap;
    }

    void findBest()
    {
        //// prune space where a partitions upper bound is smaller than global lower bound
        //CoalitionBoundMap boundMap = prune(mCoalitionBoundMap);
        IntegerPartitionBoundsMap boundMap = prune(mIntegerPartitionBoundsMap);
         
        // search next one
        if(boundMap.size() == 1)
        {
            LOG_DEBUG_S << "Best partition found: " << boundMap.begin()->first;
        } else {
            LOG_DEBUG_S << "Best partitions found: " << boundMap.size() << " left from " << mIntegerPartitionBoundsMap.size();
        }
    }

    IntegerPartition selectSubspace()
    {
        // heuristic one
        // pick space with highest upper bound
        
        // when optimality shall be traded
        // consider betaStar > 1, where betaStar = 1.05 "means that the solution sought
        // needs to hava a value that is at least 95% of the optimal one"
        //
        // select to search the smallest sub-space that could, potentially, give a value greater than 
        // or equal to (globalUpperBound/betaStar)
        //
        //      Select G (the IntegerPartition) = argumin_{G \in \mathcal{G'}: UpperBound_G \gt \frac{globalUpperBound}{{betaStar} (|P_{g}|)
        //
        // |P_{G}| -> is the size of the number of coalition structures / integer partitions for
        // the given number of coalitions



        IntegerPartition partition;
        return partition;
    }

    std::string toString()
    {
        std::stringstream ss;
        ss << "Agents: "; 
        ss << "[";
        for(size_t i = 0; i < mAgents.size(); ++i)
        {
            ss << mAgents[i];
            if(i < mAgents.size() - 1)
            {
                ss << ",";
            }
        }
        ss << "]" << std::endl;

        ss << "Bounds: " << std::endl;
        CoalitionBoundMap::const_iterator cit = mCoalitionBoundMap.begin();
        for(; cit != mCoalitionBoundMap.end(); ++cit)
        {
            ss << cit->first << " -- " << cit->second << std::endl;
        }
        return ss.str();
    }
};

} // end namespace utils
} // end namespace multiagent
#endif // MULTIAGENT_UTILS_COALITION_STRUCTURE_GENERATION_HPP
