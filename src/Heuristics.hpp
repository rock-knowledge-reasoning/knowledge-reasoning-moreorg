#ifndef ORGANIZATION_MODEL_HEURISTICS_HPP
#define ORGANIZATION_MODEL_HEURISTICS_HPP
#
#include <base/Pose.hpp>
#include "Agent.hpp"
#include "OrganizationModelAsk.hpp"

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
            size_t progressedTime);

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
        const base::Position& to);

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
        size_t availableTime);


private:
    OrganizationModelAsk mAsk;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_HEURISTICS_HPP
