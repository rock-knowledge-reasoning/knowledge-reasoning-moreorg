#include "CoalitionStructureGeneration.hpp"
#include <numeric/Combinatorics.hpp>

using namespace numeric;

namespace multiagent {
namespace utils {

std::ostream& operator<<(std::ostream& os, const AgentList& list)
{
    AgentList::const_iterator cit = list.begin();
    os << "[";
    for(; cit != list.end(); ++cit)
    {
        os << *cit;
        if(list.end() != cit + 1)
        {
            os << ",";
        }
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const CoalitionStructure& list)
{
    CoalitionStructure::const_iterator cit = list.begin();
    os << "[";
    for(; cit != list.end(); ++cit)
    {
        os << *cit;
        if(list.end() != cit + 1)
        {
            os << ",";
        }
    }
    os << "]";
    return os;
}

Bounds::Bounds(double exact)
    : maximum(exact)
    , average(exact)
    , minimum(exact)
{}

Bounds::Bounds()
    : maximum(std::numeric_limits<double>::min())
    , average(0.0)
    , minimum(std::numeric_limits<double>::max())
{}

std::string Bounds::toString() const
{
    std::stringstream ss;
    ss.precision(10);
    ss << "Bounds: max[" << maximum << "] min[" << minimum << "] average[" << average << "]";
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Bounds& bounds)
{
    os << bounds.toString();
    return os;
}

std::vector<numeric::IntegerPartition> CoalitionStructureGeneration::Statistics::remainingIntegerPartitions() const
{
    // Copy since we want to maintain order in the actual object to
    // see search and pruning order
    std::vector<numeric::IntegerPartition> all = allIntegerPartitions;
    std::vector<numeric::IntegerPartition> pruned = prunedIntegerPartitions;
    std::vector<numeric::IntegerPartition> searched = searchedIntegerPartitions;

    std::sort(all.begin(), all.end());
    std::sort(pruned.begin(), pruned.end());
    std::sort(searched.begin(), searched.end());

    std::vector<numeric::IntegerPartition>::iterator it;

    std::vector<numeric::IntegerPartition> blacklist(pruned.size() + searched.size());
    it = std::set_union(pruned.begin(), pruned.end(), searched.begin(), searched.end(), blacklist.begin());
    blacklist.resize(it - blacklist.begin());

    std::vector<numeric::IntegerPartition> remaining(all.size() + blacklist.size());
    it = std::set_difference(all.begin(), all.end(), blacklist.begin(), blacklist.end(), remaining.begin());
    remaining.resize(it - remaining.begin());

    return remaining;
}

std::string CoalitionStructureGeneration::Statistics::toString() const
{
    std::stringstream ss;
    ss << "Statistics:" << std::endl;
    ss << "    integer partitions:" << std::endl;
    ss << "        all: " << IntegerPartitioning::toString(allIntegerPartitions) << std::endl;
    ss << "        pruned: " << IntegerPartitioning::toString(prunedIntegerPartitions) << std::endl;
    ss << "        searched: " << IntegerPartitioning::toString(searchedIntegerPartitions) << std::endl;
    std::vector<numeric::IntegerPartition> remain = remainingIntegerPartitions();
    ss << "        remaining: " << IntegerPartitioning::toString(remain) << std::endl;
    return ss.str();
}

CoalitionStructureGeneration::CoalitionStructureGeneration(const AgentList& agents, CoalitionValueFunction coalitionValueFunction, CoalitionStructureValueFunction coalitionStructureValueFunction)
    : mAgents(agents)
    , mCoalitionValueFunction(coalitionValueFunction)
    , mCoalitionStructureValueFunction(coalitionStructureValueFunction)
{
    reset();
}

void CoalitionStructureGeneration::prepare()
{
    using namespace numeric;
    Combination<std::string> combinations(mAgents, mAgents.size(), MAX);
    do {

        std::vector<std::string> coalition = combinations.current();
        mAgentCoalitionMap[coalition.size()].insert(coalition);
    } while(combinations.next());


    // Compute bounds for coalitions a given size, e.g.
    // coalition of size 1: max 100, min 10, average 50
    // coalition of size 2: max 200, min 24, average 54
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

    // Compute integer partitions, e.g. for 4 agents
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
            mStatistics.allIntegerPartitions.push_back( *pit );
        }
    }

    IntegerPartitionBoundsMap updatedIntegerPartitionsBoundMap = prune(mIntegerPartitionBoundsMap);
}

double CoalitionStructureGeneration::bestLowerBound(const CoalitionBoundMap& boundMap)
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

double CoalitionStructureGeneration::bestLowerBound(const CoalitionStructureGeneration::IntegerPartitionBoundsMap& boundMap)
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

double CoalitionStructureGeneration::bestUpperBound(const IntegerPartitionBoundsMap& boundMap)
{
    double upperBound = std::numeric_limits<double>::min();
    IntegerPartitionBoundsMap::const_iterator cit = boundMap.begin();
    for(; cit != boundMap.end(); ++cit)
    {
        Bounds bounds = cit->second;
        if(bounds.maximum > upperBound)
        {
            upperBound = bounds.maximum;
        }
    }
    return upperBound;
}

Bounds CoalitionStructureGeneration::computeIntegerPartitionBounds(const IntegerPartition& partition)
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

CoalitionStructureGeneration::IntegerPartitionBoundsMap CoalitionStructureGeneration::prune(const IntegerPartitionBoundsMap& boundsMap)
{
    double globalLowerBound = bestLowerBound(boundsMap);

    IntegerPartitionBoundsMap updatedBoundsMap = boundsMap;
    IntegerPartitionBoundsMap::const_iterator cit = boundsMap.begin();

    for(; cit != boundsMap.end(); ++cit)
    {
        Bounds bounds = cit->second;
        if(bounds.maximum < globalLowerBound)
        {
            LOG_DEBUG_S << "Removing partition: " << IntegerPartitioning::toString(cit->first) << " -- max: " << bounds.maximum << " < global min: " << globalLowerBound;
            updatedBoundsMap.erase(cit->first);

            boost::unique_lock<boost::mutex> lock(mStatisticsMutex);
            mStatistics.prunedIntegerPartitions.push_back(cit->first);
        }
    }
    return updatedBoundsMap;
}


CoalitionStructureGeneration::CoalitionBoundMap CoalitionStructureGeneration::prune(const CoalitionBoundMap& boundMap)
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

void CoalitionStructureGeneration::reset()
{
    mStatistics = Statistics();
    mCurrentBestCoalitionStructure = CoalitionStructure();
    mCurrentBestCoalitionStructureValue = 0;
    mGlobalUpperBound = std::numeric_limits<double>::max();
    prepare();
}

CoalitionStructure CoalitionStructureGeneration::findBest(double quality)
{
    IntegerPartitionBoundsMap boundMap = prune(mIntegerPartitionBoundsMap);
    mGlobalUpperBound = bestUpperBound(boundMap);

    //// prune space where a partitions upper bound is smaller than global lower bound
    //CoalitionBoundMap boundMap = prune(mCoalitionBoundMap);
    while(true)
    {
        LOG_INFO_S << "Select currently best integer partition";
        IntegerPartition partition;
        try {
            partition = selectIntegerPartition(boundMap, mGlobalUpperBound);
        } catch(const std::runtime_error& e)
        {
            LOG_DEBUG_S << "No better partition found";
            break;
        }

        LOG_INFO_S << "Compute best coalition structure for this subspace: " << IntegerPartitioning::toString(partition);
        {
            boost::unique_lock<boost::mutex> lock(mStatisticsMutex);
            mStatistics.searchedIntegerPartitions.push_back(partition);
        }
        bool improvedResult = searchSubspace(partition, 0, 0, mAgents, CoalitionStructure(), quality);

        // No improvement of the results
        if(!improvedResult)
        {
            boundMap.erase(partition);
            mGlobalUpperBound = bestUpperBound(boundMap);
        } else {
            if(currentBestSolutionQuality() <= quality)
            {
                LOG_INFO_S << "Good enough coalition found: " << currentBestSolution() << ", value " << currentBestSolutionValue();
                break;
            }

            Bounds bounds( currentBestSolutionValue() );
            boundMap[partition] = bounds;

            LOG_DEBUG_S << "Best coalition found so far: " << currentBestSolution() << ", " << bounds;

            // TODO: pick the next one for evaluation and repeat until satified with
            // the solution
            boundMap = prune(boundMap);
            mGlobalUpperBound = bestUpperBound(boundMap);
            boundMap.erase(partition);
        }

        if(boundMap.empty())
        {
            break;
        }
    }
    mCompletionTime = base::Time::now();

    LOG_DEBUG_S << "Best coalition found: " << currentBestSolution() << ", " << currentBestSolutionValue();
    return currentBestSolution();
}

void CoalitionStructureGeneration::anytimeSearch(double quality)
{
    mStartTime = base::Time::now();
    mCompletionTime = base::Time();
    mThread = boost::thread(&CoalitionStructureGeneration::findBest, this, quality);
}

void CoalitionStructureGeneration::stopSearch()
{
    mThread.interrupt();
}

CoalitionStructure CoalitionStructureGeneration::currentBestSolution() const
{
    boost::unique_lock<boost::mutex> lock(mSolutionMutex);
    return mCurrentBestCoalitionStructure;
}

double CoalitionStructureGeneration::currentBestSolutionValue() const
{
    boost::unique_lock<boost::mutex> lock(mSolutionMutex);
    return mCurrentBestCoalitionStructureValue;
}

double CoalitionStructureGeneration::currentBestSolutionQuality() const
{
    return mGlobalUpperBound / currentBestSolutionValue();
}

IntegerPartition CoalitionStructureGeneration::selectIntegerPartition(const CoalitionStructureGeneration::IntegerPartitionBoundsMap& boundMap, double maximumBound) const
{
    // heuristic one
    // pick space with highest upper bound
    IntegerPartitionBoundsMap::const_iterator it = boundMap.begin();
    double value = 0.0;
    IntegerPartition partition;
    for(; it != boundMap.end(); ++it)
    {
        LOG_INFO_S << "Current: " << IntegerPartitioning::toString(partition) << ", value " << value;
        LOG_INFO_S << "Check: " << IntegerPartitioning::toString(it->first) << ", value " << it->second.maximum;
        if(it->second.maximum > value)
        {
            partition = it->first;
            value = it->second.maximum;
        }
    }
    if(value == 0.0)
    {
        throw std::runtime_error("No integer partition found greater than the given maximum bound");
    } else {
        return partition;
    }

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
}

/**
 *
 * \param globalUpperBound
 * \param bestStar Quality of the solution, i.e. 1.05 means 95% percent of the optimal solution
 */
bool CoalitionStructureGeneration::searchSubspace(const IntegerPartition& partition, size_t k, size_t alpha, const AgentList& agents, const CoalitionStructure& currentStructure, double betaStar)
{
    bool improvedResult = false;

    std::string indent(4*k, ' ');
    LOG_DEBUG_S << indent << " search subspace of current structure: " << currentStructure;

    if(k > 0 && partition[k] != partition[k-1])
    {
        LOG_DEBUG_S << indent << " search subspace for k: " << k << ", partition[k]: " << partition[k] << " != " << partition[k-1];
        // resetting alpha when the size is not repeated
        alpha = 1;
    }

    // Initialize index list
    std::vector<int> indexList;
    for(size_t i = 0; i < agents.size(); ++i)
    {
        indexList.push_back(i);
    }
    LOG_DEBUG_S << indent << " indexlist initialized: " << IntegerPartitioning::toString(indexList) << " agents: " << agents.size();

    // Compute upper bound for M_{k,0} to avaoid redundant computations
    size_t upperBoundM_k = mAgents.size() + 1;
    for(size_t i = 0; i < k; ++i)
    {
        upperBoundM_k += partition[i]*IntegerPartitioning::multiplicity(partition, partition[i]);
    }
    LOG_DEBUG_S << indent << " upperBound M_k: " << upperBoundM_k;

    // Compute upper bound of subspace, so that we can stop computation when this maximum has
    // been found
    double upperBoundOfSubspace = mIntegerPartitionBoundsMap[partition].maximum;
    LOG_DEBUG_S << indent << " upperBound of subspace " << IntegerPartitioning::toString(partition) << ": " << upperBoundOfSubspace;

    using namespace numeric;
    Combination<int> combinations(indexList, partition[k], EXACT);
    do {
        std::vector<int> m_k = combinations.current();
        LOG_DEBUG_S << indent << " current combination m_k=" << IntegerPartitioning::toString(m_k) << ", alpha=" << alpha;
        // m_k[0] + 1: we start with index 0
        if(alpha <= m_k[0] + 1 && m_k[0] + 1 <= upperBoundM_k)
        {
            LOG_DEBUG_S << indent << " computing combination";
            // Map combination of indices to current coalition, i.e. an agent (sub)set
            AgentList remainingAgents = agents;
            std::vector<Agent> coalition;
            for(size_t i = 0; i < m_k.size(); ++i)
            {
                Agent agent = agents[ m_k[i] ];
                coalition.push_back(agent);
                std::vector<Agent>::iterator it = std::find(remainingAgents.begin(), remainingAgents.end(), agent);
                remainingAgents.erase(it);
            }
            CoalitionStructure coalitionStructure = currentStructure;
            coalitionStructure.push_back(coalition);

            LOG_DEBUG_S << indent << " current coalition structure: " << currentStructure;
            LOG_DEBUG_S << indent << " partial coalition structure: " << coalitionStructure << " , remaining agents " << remainingAgents;

            double currentStructureValue = mCoalitionStructureValueFunction( coalitionStructure );

            // Check if we reached the end
            if(k == partition.size() - 1)
            {
                LOG_DEBUG_S << indent << " reached the end at k: " << k << " -- bestValue: " << currentBestSolution() << " vs. " << currentStructureValue;
                if(currentStructureValue > currentBestSolutionValue())
                {
                    // update the currently best coalition structure
                    LOG_DEBUG_S << indent << " update global best: " << currentBestSolution() << ", value " << currentBestSolutionValue();
                    improvedResult = updateCurrentBestCoalitionStructure(coalitionStructure, currentStructureValue) || improvedResult;
                }
            } else {
                LOG_DEBUG_S << indent << " computing subspace potential";
                // Estimate the potential of this subspace using
                // know value for the already found coalitions
                // and existing upper bounds for the coalitions still
                // to look at
                double subspacePotentialValue = 0;

                // Estimate value of current (partial) coalition structure
                for( size_t i = 0; i < coalitionStructure.size(); ++i)
                {
                    double valueOfCoalition = mCoalitionValueFunction( coalitionStructure[i] );
                    LOG_DEBUG_S << indent << " coalition: " << coalitionStructure[i] << " value: " << valueOfCoalition;
                    subspacePotentialValue += valueOfCoalition;
                }

                // Estimate value for the rest of the partition based on the bounds computed
                // on coalition sizes, i.e.
                // looking at the integer partition's yet uninvestigate range
                for( size_t i = coalitionStructure.size(); i < partition.size(); ++i)
                {
                    double max_s = mCoalitionBoundMap[ partition[i] ].maximum;
                    LOG_DEBUG_S << indent << " potential for coalition size: " << partition[i] << ": " << max_s;
                    subspacePotentialValue += max_s;
                }
                LOG_DEBUG_S << indent << " subspace potential: " << subspacePotentialValue;

                // Check if it is worth to enter this subspace
                if(currentBestSolutionValue() < subspacePotentialValue)
                {
                    LOG_DEBUG_S << indent << " continue search: best: " << currentBestSolutionValue() << " vs. subspace potential: " <<  subspacePotentialValue;
                    if( searchSubspace(partition, k+1, m_k[0], remainingAgents, coalitionStructure, betaStar) )
                    {
                        improvedResult = true;
                    }
                } else {
                    LOG_DEBUG_S << indent << " pruning: insufficient subspace potential: best: " << currentBestSolutionValue() << " vs. " <<  subspacePotentialValue;
                }
            }

            // Stop if the required solution has been found or if the current best
            // is equal to the upper bound of this sub-space
            if( currentBestSolutionQuality() <= betaStar )
            {

                LOG_DEBUG_S << indent << " prematurely returning best structure: " << currentBestSolution();
                LOG_DEBUG_S << "Quality sufficient: " << currentBestSolutionQuality() << "<=" << betaStar;
                return improvedResult;
            }

            if(currentBestSolutionValue() == upperBoundOfSubspace)
            {
                LOG_DEBUG_S << indent << " prematurely returning best structure: " << currentBestSolution();
                LOG_DEBUG_S << indent << " bestStructureValue == upperBoundOfSubspace, both are " << currentBestSolutionValue();
                return improvedResult;
            }
        } else {
        // if(alpha <= m_k[0] && m_k[0] <= upperBoundM_k)
            LOG_DEBUG_S << indent << " skipping: condition does not hold: alpha < m_k[0]+1 && m_k[0]+1 <= upperBoundM_k"
                << ", alpha= " << alpha << ", m_k[0]=" << m_k[0] << ", upperBoundM_k=" << upperBoundM_k;
        }
    } while(combinations.next());

    return improvedResult;
}

bool CoalitionStructureGeneration::updateCurrentBestCoalitionStructure(const CoalitionStructure& coalitionStructure, double value)
{
    boost::unique_lock<boost::mutex> lock(mSolutionMutex);
    if(value > mCurrentBestCoalitionStructureValue)
    {
        mCurrentBestCoalitionStructureValue = value;
        mCurrentBestCoalitionStructure = coalitionStructure;
        return true;
    }

    return false;
}

std::string CoalitionStructureGeneration::toString() const
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

    ss << "Current best solution: " << std::endl;
    ss << toString(mCurrentBestCoalitionStructure);
    return ss.str();
}

std::string CoalitionStructureGeneration::toString(const CoalitionStructure& c)
{
    std::stringstream ss;
    ss << c;
    return ss.str();
}

CoalitionStructureGeneration::Statistics CoalitionStructureGeneration::getStatistics() const
{
    boost::unique_lock<boost::mutex> lock(mStatisticsMutex);
    return mStatistics;
}

} // end namespace utils
} // end namespace multiagent
