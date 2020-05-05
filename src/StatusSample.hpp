#ifndef ORGANIZATION_MODEL_STATUS_SAMPLE_HPP
#define ORGANIZATION_MODEL_STATUS_SAMPLE_HPP

#include <vector>
#include <map>
#include <base/Pose.hpp>
#include "Agent.hpp"
#include "Resource.hpp"
#include "Types.hpp"
#include "OrganizationModelAsk.hpp"
#include "Sample.hpp"

namespace moreorg {

/**
 * \class StatusSample
 * \brief A StatusSample describes the status of an agent (composite) for a
 * certain time interval
 * \details The status sample serve as basis to (a) monitor a system, and (b)
 * analyse a plan of a system
 */
class StatusSample : public Sample
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
     */
    StatusSample(const Agent& instance,
            const base::Position& fromLocation,
            const base::Position& toLocation,
            size_t fromTime,
            size_t toTime,
            Agent::OperationalStatus operationalStatus,
            activity::Type activity
    );

    virtual ~StatusSample();

    const Agent& getAgent() const { return mAgent; }
    Agent::OperationalStatus getOperationalStatus() const { return mOperationalStatus; }
    activity::Type getActivityType() const { return mActivityType; }

private:
    Agent mAgent;
    Agent::OperationalStatus mOperationalStatus;
    activity::Type mActivityType;
    Resource::Set mResourceRequirements;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_STATUS_SAMPLE_HPP
