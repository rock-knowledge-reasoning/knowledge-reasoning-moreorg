#ifndef ORGANIZATION_MODEL_STATUS_SAMPLE_HPP
#define ORGANIZATION_MODEL_STATUS_SAMPLE_HPP

#include <vector>
#include <map>
#include <base/Pose.hpp>
#include "Agent.hpp"
#include "Resource.hpp"
#include "Types.hpp"
#include "OrganizationModelAsk.hpp"

namespace organization_model {

/**
 * \class StatusSample
 * \brief A StatusSample describes the status of an agent (composite) for a
 * certain time interval
 * \details The status sample serve as basis to (a) monitor a system, and (b)
 * analyse a plan of a system
 */
class StatusSample
{
public:
    typedef std::vector<StatusSample> List;
    typedef std::vector<const StatusSample*> ConstRawPtrList;
    typedef std::map<const StatusSample*, double> RawPtr2Double;

    StatusSample();

    /**
     * \brief StatusSample constructor
     * \details A status sample represent
     * \param instance The (composite) agent
     * \param fromLocation the location the agent is at time fromTime
     * \param toTime the location the agent is at time toTime
     * \param operationalStatus Operational status of the agent as operative or
     * dormant
     * \param activity The type of activity if the agent is operative
     * \param resourceRequirement actively used resources for the activity
     */
    StatusSample(const Agent& instance,
            const base::Position& fromLocation,
            const base::Position& toLocation,
            size_t fromTime,
            size_t toTime,
            Agent::OperationalStatus operationalStatus,
            activity::Type activity,
            const Resource::Set& resourceRequirements);

    const Agent& getAgent() const { return mAgent; }
    const base::Position& getFromLocation() const { return mFromLocation; }
    const base::Position& getToLocation() const { return mToLocation; }
    size_t getFromTime() const { return mFromTime; }
    size_t getToTime() const { return mToTime; }
    size_t getAvailableTime() const { return mToTime - mFromTime; }
    Agent::OperationalStatus getOperationalStatus() const { return mOperationalStatus; }
    activity::Type getActivityType() const { return mActivityType; }
    const Resource::Set& getResourceRequirements() const { return mResourceRequirements; }

    /**
     * Check if the status sample intersects with the given time
     * \return true if \p time lies in the interval of getFromTime and
     * getToTime
     */
    bool matchesTime(size_t time) const { return time >= mFromTime && time <= mToTime; }

private:
    Agent mAgent;
    base::Position mFromLocation;
    base::Position mToLocation;
    size_t mFromTime;
    size_t mToTime;
    Agent::OperationalStatus mOperationalStatus;
    activity::Type mActivityType;
    Resource::Set mResourceRequirements;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_STATUS_SAMPLE_HPP
