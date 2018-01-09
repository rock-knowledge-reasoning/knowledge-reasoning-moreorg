#ifndef ORGANIZATION_MODEL_HEURISTICS_HPP
#define ORGANIZATION_MODEL_HEURISTICS_HPP
#
#include <base/Pose.hpp>
#include "Agent.hpp"
#include "OrganizationModelAsk.hpp"
#include "StatusSample.hpp"

namespace organization_model {

/**
 * \class Heuristics
 * \brief Collection of heuristic functions
 */
class Heuristics
{
public:
    /**
     * Default constructor
     * \param ask OrganizationModelAsk which contains the available model pool
     * and thus allows to access agent model information, e.g. to account for
     * the nominal velocity of an agent for travel
     */
    Heuristics(const OrganizationModelAsk& ask);

    /**
     * Position estimate based on linear interpolation
     * and using the nominal velocity of the agent
     * \param agent Agent that travels from position \p from to position \p to
     * \param from from position
     * \param to to position
     * \param progressedTime time progressed when leaving from \p from
     * \return estimated position
     */
    base::Position positionLinear(const Agent& agent,
            const base::Position& from,
            const base::Position& to,
            size_t progressedTime) const;

    /**
     * Time estimate for the required travel of agent \p agent from
     * position \p from to position \p to
     * \param agent that travels from position \p from to position \p to
     * \param from from position
     * \param to to position
     * \return estimated travel time
     */
    double travelTime(const Agent& agent,
        const base::Position& from,
        const base::Position& to) const;

    double travelTime(const StatusSample* sample) const;

    /**
     * Wait time estimate for the travel between \p from and \p to, where
     * the overall \p availableTime is given
     * \param agent Agent that travels from position \p from to position \p to
     * \param from from position
     * \param to to position
     * \param availableTime time that is available to perform this travel
     * \return estimated wait time
     */
    double waitTime(const Agent& agent,
        const base::Position& from,
        const base::Position& to,
        size_t availableTime) const;

    /**
     * Estimate the wait time for a given sample
     * \param sample
     */
    double waitTime(const StatusSample* sample) const;

    /**
     * Estimate the overall energy consumption for a given sample
     * \param sample
     */
    double getEnergyConsumption(const StatusSample* sample) const;

    /**
     * Compute the energy reduction for a particular atomic agent for operation
     * from \p fromTime to \p toTime
     * \return reduction
     */
    double getEnergyReductionAbsolute(const StatusSample* sample,
            const AtomicAgent& agent,
            size_t fromTime,
            size_t toTime) const;

    /**
     * Compute the reconfiguration cost from a set of agent to another set of
     * agents, based on teach target agent and involved source agents
     *
     * One could consider using the pddl planner, but since the split
     * disassembles the complete system, this does not give a correct estimate
     *
     * \see getReconfigurationCost
     */
    double getReconfigurationCost(const Agent::Set& from,
            const Agent::Set& to) const;

    /**
     * Compute the reconfiguration to form a particular target agent from a set
     * of source agents
     *
     * Compute cost based on |source-agents|*baseFactor + sum of involved_agents
     *
     * \param target Target Agent to be formed
     * \param origins Map of source agents and the relevant involved atomic
     * \param cooperationTimeInS The basic time to align to system for
     * cooperation in seconds
     * \param transferTimePerAtomicAgentInS The basic time for transfer an
     * atomic agent in seconds
     * agents
     */
    double getReconfigurationCost(const Agent& target,
            const std::map<Agent, AtomicAgent::List>& origins,
            double cooperationTimeInS = 600,
            double transferTimePerAtomicAgentInS = 180)
        const;
private:
    OrganizationModelAsk mAsk;

    // Cached data
    mutable StatusSample::RawPtr2Double mEnergyConsumption;
    mutable StatusSample::RawPtr2Double mTravelTime;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_HEURISTICS_HPP
