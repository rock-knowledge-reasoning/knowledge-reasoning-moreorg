#ifndef ORGANIZATION_MODEL_ANALYSER_HPP
#define ORGANIZATION_MODEL_ANALYSER_HPP

#include "OrganizationModelAsk.hpp"
#include "StatusSample.hpp"
#include "Heuristics.hpp"

namespace organization_model {

/**
 * \class Analyser
 * \brief Analysis of the state of an organization model
 * \details The analyser is feed with the full state description
 * of an organization, i.e. positions over time and a description of
 * the activities
 */
class Analyser
{
public:
    typedef std::vector<StatusSample::ConstRawPtrList> SampleIndex;

    Analyser(const OrganizationModelAsk& ask);

    /**
     * Add a status sample, where a status sample describe the agent/coalition
     * and the corresponding activity and required functionality
     */
    void add(const StatusSample& sample);

    /**
     * Create the index after adding the final status sample
     */
    void createIndex();

    /**
     * Retrieve all samples at a given time point
     */
    StatusSample::ConstRawPtrList samplesAt(size_t time) const;

    /**
     * Retrieve all samples for a given atomic agent
     */
    StatusSample::ConstRawPtrList samplesFor(const AtomicAgent& atomicAgent) const;

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
     * Get the current activities at time t
     * \param time
     */
    std::vector<activity::Type> getActivities(size_t time) const;

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
    std::vector<double> getEnergyReductionAbsolute(size_t time) const;

    /**
     * Get absolute available energy at \p time for the active coalition
     * structure
     * \param time
     * \return list of values which corresponding to the current coalition
     * structure
     */
    std::vector<double> getEnergyAvailableAbsolute(size_t time) const;

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
     * capacity at \p time
     * \param time
     * \return list of values which corresponding to the current coalition
     * structure
     * \see getCoalitionStructure
     */
    std::vector<double> getEnergyAvailableRelative(size_t time) const;

    /**
     * Compute the median of a number of values
     * \return median of values
     */
    static double getMedian(std::vector<double> values);

    /**
     * Get the energy distribution as median absolute deviation (MAD)
     * \return median absolute deviation at \p time
     */
    double getEnergyMAD(size_t time) const;

    /**
     * Get the min relative available energy at \p time
     * This allows to identify critical energy levels
     * \return minimal available energy (relative to an agents full capacity)
     */
    double getEnergyAvailableMinRelative(size_t time) const;

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

private:
    /// Resources
    OrganizationModelAsk mAsk;
    Heuristics mHeuristics;
    StatusSample::List mSamples;
    size_t mMinTime;
    size_t mMaxTime;

    /// Sample
    SampleIndex mTimeIndexedSamples;
    /// Map of atomic agents to related samples -- since no parallel activities
    /// are possible a sample list can be used
    std::map<AtomicAgent, StatusSample::ConstRawPtrList> mAtomicAgentIndexedSamples;

    /// indexing: fromTime - ToTime: key
    // queries:
    //     get information about atomic agent at time t
    //     get information about composite agents at time t


};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ANALYSER_HPP
