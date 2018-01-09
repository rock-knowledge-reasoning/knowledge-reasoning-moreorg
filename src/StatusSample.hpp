#ifndef ORGANIZATION_MODEL_STATUS_SAMPLE_HPP
#define ORGANIZATION_MODEL_STATUS_SAMPLE_HPP

#include <vector>
#include <map>
#include <base/Pose.hpp>
#include "Agent.hpp"
#include "FunctionalityRequirement.hpp"
#include "Types.hpp"
#include "OrganizationModelAsk.hpp"

namespace organization_model {

/**
 * \class StatusSample
 * \brief A StatusSample describes the status of an agent (composite) for a
 * certain time interval
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
     */
    StatusSample(const Agent& instance,
            const base::Position& fromLocation,
            const base::Position& toLocation,
            size_t fromTime,
            size_t toTime,
            bool active,
            activity::Type activity,
            const FunctionalityRequirement& functionalityRequirement);

    const Agent& getAgent() const { return mAgent; }
    const base::Position& getFromLocation() const { return mFromLocation; }
    const base::Position& getToLocation() const { return mToLocation; }
    size_t getFromTime() const { return mFromTime; }
    size_t getToTime() const { return mToTime; }
    size_t getAvailableTime() const { return mToTime - mFromTime; }
    bool isActive() const { return mIsActive; }
    activity::Type getActivityType() const { return mActivityType; }
    FunctionalityRequirement getFunctionalityRequirement() const { return mFunctionalityRequirement; }

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
    bool mIsActive;
    activity::Type mActivityType;
    FunctionalityRequirement mFunctionalityRequirement;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_STATUS_SAMPLE_HPP
