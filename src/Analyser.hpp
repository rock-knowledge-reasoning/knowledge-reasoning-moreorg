#ifndef ORGANIZATION_MODEL_ANALYSER_HPP
#define ORGANIZATION_MODEL_ANALYSER_HPP

#include "OrganizationModelAsk.hpp"
#include "StatusSample.hpp"
#include "RequirementSample.hpp"
#include "Heuristics.hpp"
#include "Metric.hpp"
#include <numeric/Stats.hpp>

namespace organization_model {

/**
 * \class Analyser
 * \brief Analysis of the state of an organization model
 * \details The analyser is feed with the full state description
 * of an organization, i.e. positions over time and a description of
 * the activities
 *
 * All function which only use \a time as argument, will refer to
 * the organization, otherwise the following suffixes are used: A: Agent, AA: AtomicAgent, O: Organization.
 */
class Analyser
{
public:
    typedef std::vector<StatusSample::ConstRawPtrList> StatusSampleIndex;
    typedef std::vector<RequirementSample::ConstRawPtrList> RequirementSampleIndex;

    typedef std::function<double(size_t, const AtomicAgent&)> AtomicAgentSampleFunc;
    typedef std::function<double(size_t, const Agent&)> AgentSampleFunc;
    typedef std::function<double(size_t)> OrganizationSampleFunc;
    /// Function to compute value vector for a coalition structure
    //typedef std::function< std::vector<double>(size_t, const Agent&) > CoalitionStructureValueFunc;


    /// Sample types for atomic agents
    enum AtomicAgentSampleType {
        AA_EnergyReductionAbsolute,
        AA_EnergyReductionRelative,
        AA_TravelDistance,
        AA_TravelDistanceToNextPOI,
        AA_OperativeTime,
        AA_DormantTime,
        AA_Redundancy
    };

    static std::map<AtomicAgentSampleType,std::string> AtomicAgentSampleTypeTxt;

    /// Sample types for (composite) agents
    enum AgentSampleType {
        A_EnergyReductionAbsolute,
        A_EnergyReductionRelative,
        A_EnergyAvailableAbsolute,
        A_EnergyAvailableRelative,
        A_Redundancy
    };

    static std::map<AgentSampleType,std::string> AgentSampleTypeTxt;

    /// Sample types for organizations
    enum OrganizationSampleType {
        O_EnergyMedianAbsoluteDeviation,
        O_EnergyAvailableMinRelative,
        O_RedundancyMin,
        O_RedundancyMax
    };

    static std::map<OrganizationSampleType,std::string> OrganizationSampleTypeTxt;

    /**
     * Default constructor
     * \param ask
     */
    Analyser(const OrganizationModelAsk& ask);

    /**
     * Add requirement sample
     */
    void add(const RequirementSample& sample);

    /**
     * Add a status sample, where a status sample describe the agent/coalition
     * and the corresponding activity and required functionality
     */
    void add(const StatusSample& sample);

    /**
     * Create the sample index after adding the final status sample
     */
    void createIndex();

    /**
     * Retrieve all samples at a given time point
     */
    StatusSample::ConstRawPtrList statusSamplesAt(size_t time) const;

    /**
     * Retrieve all samples for a given atomic agent
     */
    StatusSample::ConstRawPtrList statusSamplesFor(const AtomicAgent& atomicAgent) const;

    /**
     * Retrieve all samples at a given time point
     */
    RequirementSample::ConstRawPtrList requirementSamplesAt(size_t time) const;

    // QUERY INTERFACE
    // from agents derive static properties:
    //     mass
    //

    /**
     * Dynamic / time dependant agents at time t
     * \param time
     *
     */
    Agent::List getCoalitionStructure(size_t time) const;

    /**
     * Get the positions of all agents
     * \param time
     * \return
     */
    std::vector<base::Position> getPositions(size_t time) const;

    /**
     * Get the functional requirements at given time
     */
    std::vector<Resource::Set> getRequirements(size_t time) const;


    /**
     * Get the current activities at time t
     * \param time
     */
    std::vector<activity::Type> getActivities(size_t time) const;

    // UNIQUE SIGNATURES TO ALLOW THE USAGE OF BIND
    double getEnergyReductionAbsoluteAA(size_t time, const AtomicAgent& atomicAgent) const
    {
        return getEnergyReductionAbsolute(time, atomicAgent);
    }
    double getEnergyReductionRelativeAA(size_t time, const AtomicAgent& atomicAgent) const
    {
        return getEnergyReductionRelative(time, atomicAgent);
    }
    double getTravelDistanceAA(size_t time, const AtomicAgent& atomicAgent) const
    {
        return getTravelDistance(time, atomicAgent);
    }
    double getTravelDistanceToNextPOIAA(size_t time, const AtomicAgent& atomicAgent) const
    {
        return getTravelDistanceToNextPOI(time, atomicAgent);
    }
    double getOperativeTimeAA(size_t time, const AtomicAgent& atomicAgent) const
    {
        return getOperativeTime(time, atomicAgent);
    }
    double getDormantTimeAA(size_t time, const AtomicAgent& atomicAgent) const
    {
        return getDormantTime(time, atomicAgent);
    }

    /**
     * Select required columns for the atomic agent sample
     */
    void setSampleColumnsAA(const std::vector<AtomicAgentSampleType>& columnTypes);

    std::string getSampleColumnDescriptionAA() const;

    /**
     * Retrieve sample (row) for an atomic agent
     */
    std::vector<double> getSampleAA(size_t time, const AtomicAgent& atomicAgent) const;

    double getEnergyReductionAbsoluteA(size_t time, const Agent& agent) const
    {
        return getEnergyReductionAbsolute(time, agent);
    }
    double getEnergyReductionRelativeA(size_t time, const Agent& agent) const
    {
        return getEnergyReductionRelative(time, agent);
    }
    double getEnergyAvailableAbsoluteA(size_t time, const Agent& agent) const
    {
        return getEnergyAvailableAbsolute(time, agent);
    }
    double getEnergyAvailableRelativeA(size_t time, const Agent& agent) const
    {
        return getEnergyAvailableRelative(time, agent);
    }

    /**
     * Select required columns for the atomic agent sample
     */
    void setSampleColumnsA(const std::vector<AgentSampleType>& columnTypes);

    std::string getSampleColumnDescriptionA() const;

    /**
     * Retrieve sample (row) for an atomic agent
     */
    std::vector<double> getSampleA(size_t time, const Agent& agent) const;

    /**
     * Select required columns for the orgamization
     */
    void setSampleColumnsO(const std::vector<OrganizationSampleType>& columnTypes);

    std::string getSampleColumnDescriptionO() const;

    /**
     * Retrieve sample (row) for an organization
     */
    std::vector<double> getSampleO(size_t time) const;


    /**
     * Get the energy reduction for an individual agent, when assuming the given
     * sample and associated time interval
     * p/atomic agent: energy reduction current / energy max
     * p/agent: energy current / energy max
     * p/agent: energy distribution / entropy
     * p/coalition structure: energy distribution
     *
     * \param sample
     * \param atomicAgent
     * \param fromTime
     * \param toTime
     * \return absolute energy reduction
     */
    double getEnergyReductionAbsolute(const StatusSample* sample, const AtomicAgent& atomicAgent, size_t fromTime, size_t toTime) const;

    /**
     * Get the absolute energy reduction at \p time for \p atomicAgent
     * \param time
     * \param atomicAgent
     * \return energy reduction
     */
    double getEnergyReductionAbsolute(size_t time, const AtomicAgent& atomicAgent) const;

    /**
     * Get the absolute energy reduction at \p time for \p agent
     * \param time
     * \param agent
     * \return energy reduction
     */
    double getEnergyReductionAbsolute(size_t time, const Agent& agent) const;

    /**
     * Get absolute available energy at \p time for \p agent
     * \param time
     * \param agent
     * \return available energy
     */
    double getEnergyAvailableAbsolute(size_t time, const Agent& agent) const;


    /**
     * Get absolute energy reduction at \p time
     * \param time
     * \return list of values which corresponding to the current coalition
     * structure
     */
    std::vector<double> getEnergyReductionAbsolute(size_t time) const { return getCoalitionStructureValue(time, A_EnergyReductionAbsolute); }

    /**
     * Get absolute available energy at \p time for the active coalition
     * structure
     * \param time
     * \return list of values which corresponding to the current coalition
     * structure
     */
    std::vector<double> getEnergyAvailableAbsolute(size_t time) const { return getCoalitionStructureValue(time, A_EnergyAvailableAbsolute); }

    /**
     * Compute the remaining energy relative to the full energy capacity of the
     * given agent
     * \param time
     * \param agent
     * \return relative available energy
     */
    double getEnergyAvailableRelative(size_t time, const Agent& agent) const;

    /**
     * Compute the energy reduction relative to the full capacity of an agent
     * \param time
     * \param agent
     * \return relative energy reduction
     */
    double getEnergyReductionRelative(size_t time, const Agent& agent) const;

    /**
     * Compute the energy reduction relative to the full capacity of an atomic
     * agent
     * \param time
     * \param atomicAgent
     * \return relative energy reduction
     */
    double getEnergyReductionRelative(size_t time, const AtomicAgent& atomicAgent) const;

    /**
     * Compute the relative available energy (with respect to agents' full
     * capacity at \p time)
     * \param time
     * \return list of values which corresponding to the current coalition
     * structure
     * \see getCoalitionStructure
     */
    std::vector<double> getEnergyAvailableRelative(size_t time) const { return getCoalitionStructureValue(time, A_EnergyAvailableRelative); }


    /**
     * Compute the value vector for a coalition structure, based on a given
     * value function
     * \param time
     * \param type AgentSampleType
     * \return coalition structure vector
     */
    std::vector<double> getCoalitionStructureValue(size_t time, AgentSampleType type) const;

    /**
     * Compute the median of a number of values
     * \return median of values
     */
    static double getMedian(std::vector<double> values);

    static double getMedianAbsoluteDeviation(const std::vector<double>& values);

    static double getMin(const std::vector<double>& values) { return getStats(values).min(); }
    static double getMax(const std::vector<double>& values) { return getStats(values).max(); }
    static double getMean(const std::vector<double>& values) { return getStats(values).mean(); }
    static double getStdev(const std::vector<double>& values) { return getStats(values).stdev(); }

    static numeric::Stats<double> getStats(const std::vector<double>& values);

    /**
     * Get the energy distribution as median absolute deviation (MAD)
     * \return median absolute deviation at \p time
     */
    double getEnergyMAD(size_t time) const { return getMedianAbsoluteDeviation(getEnergyAvailableRelative(time)); }

    /**
     * Get the min relative available energy at \p time
     * This allows to identify critical energy levels
     * \return minimal available energy (relative to an agents full capacity)
     */
    double getEnergyAvailableMinRelative(size_t time) const { return getMin( getEnergyAvailableRelative(time) ); }

    /**
     * Get the minimum redundancy
     */
    double getMinRedundancy(size_t time) const { return getMin( getRedundancy(time) ); }
    /**
     * Get the maximum redundancy
     */
    double getMaxRedundancy(size_t time) const { return getMax( getRedundancy(time) ); }

    // atomic agents:
    // - travelledDistance
    // - wait/stand time
    // remaining travel distance p atomic agent / p composite agent

    /**
     * Compute the travel distance, based on the direct line between two
     * locations for a particular atomic agent and the set of involved travel
     * segments
     */
    double getTravelDistance(size_t time, const AtomicAgent& atomicAgent) const;

    /**
     * Get the overall travel distance of an atomic agent
     * \return travel distance
     */
    double getTravelDistance(const AtomicAgent& atomicAgent) const;

    /**
     * Get the travel distance to the next travel point here: Point Of Interest
     * (POI)
     * \return distance to next POI
     */
    double getTravelDistanceToNextPOI(size_t time, const AtomicAgent& atomicAgent) const;

    /**
     * Get operative time, i.e., when the functionality of this atomic agent
     * is required
     * TBD: how to compute, this must be based on the set of active agents over
     * the course of a given solution
     *
     */
    double getOperativeTime(size_t time, const AtomicAgent& atomicAgent) const;

    /**
     * Get dormant time, i.e., when the functionality of this atomic agent is
     * not required and does not need to be maintained
     */
    double getDormantTime(size_t time, const AtomicAgent& atomicAgent) const;

    static std::string toString(const std::vector<double>& data);

    /**
     * TBD: Is there a difference to dormant?!
     */
    // double getWaitTime(size_t time, const AtomicAgent& atomicAgent) const;


    // ORGANIZATIONAL METRICS
    // to be applied on top of this interface
    // e.g. average between nearest neighbors
    // median absolute deviation (MAD), average absolute deviation, gini
    // https://en.wikipedia.org/wiki/Index_of_dispersion: variance / mean, mean
    // must be non-zero
    // https://en.wikipedia.org/wiki/Median_absolute_deviation
    // https://en.wikipedia.org/wiki/Spatial_descriptive_statistics#Measures_of_spatial_dispersion

    std::vector<double> getMetric(size_t time, metrics::Type metric) const;


    /**
     * Compute redundancies for the coalition structure Compute redundancies for
     * the coalition structure
     */
    std::vector<double> getRedundancy(size_t time) const;

    /**
     * Return metric to compute safety property
     */
    Metric::Ptr getMetric(metrics::Type metric = metrics::REDUNDANCY) const { return Metric::getInstance(metric, mAsk); }

    /**
     * Get the heuristic
     */
    const Heuristics& getHeuristics() const { return mHeuristics; }

private:
    /// Resources
    OrganizationModelAsk mAsk;
    Heuristics mHeuristics;

    // TODO: include metric in general


    /// Sample of the actual status
    StatusSample::List mStatusSamples;
    size_t mStatusMinTime;
    size_t mStatusMaxTime;

    /// Samples of the requirements
    RequirementSample::List mRequirementSamples;
    size_t mRequirementMinTime;
    size_t mRequirementMaxTime;

    /// StatusSample
    StatusSampleIndex mTimeIndexedStatusSamples;
    RequirementSampleIndex mTimeIndexedRequirementSamples;
    /// Map of atomic agents to related samples -- since no parallel activities
    /// are possible a sample list can be used
    std::map<AtomicAgent, StatusSample::ConstRawPtrList> mAtomicAgentIndexedSamples;

    /// Sampling function depending on the sample time
    std::map<AtomicAgentSampleType, AtomicAgentSampleFunc> mAtomicAgentSampleFunctionMap;
    std::vector<AtomicAgentSampleType> mAtomicAgentSampleTypes;
    std::vector<AtomicAgentSampleFunc> mAtomicAgentSampleFunctions;

    std::map<AgentSampleType, AgentSampleFunc> mAgentSampleFunctionMap;
    std::vector<AgentSampleType> mAgentSampleTypes;
    std::vector<AgentSampleFunc> mAgentSampleFunctions;

    std::map<OrganizationSampleType, OrganizationSampleFunc> mOrganizationSampleFunctionMap;
    std::vector<OrganizationSampleType> mOrganizationSampleTypes;
    std::vector<OrganizationSampleFunc> mOrganizationSampleFunctions;

    /// indexing: fromTime - ToTime: key
    // queries:
    //     get information about atomic agent at time t
    //     get information about composite agents at time t


};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ANALYSER_HPP
