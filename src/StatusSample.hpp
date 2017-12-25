#ifndef ORGANIZATION_MODEL_STATUS_SAMPLE_HPP
#define ORGANIZATION_MODEL_STATUS_SAMPLE_HPP

#include <vector>
#include <base/Pose.hpp>
#include "Agent.hpp"
#include "FunctionalityRequirement.hpp"
#include "Types.hpp"

namespace organization_model {

class StatusSample
{
public:
    typedef std::vector<StatusSample> List;
    typedef std::vector<const StatusSample*> ConstRawPtrList;

    StatusSample();

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
    bool isActive() const { return mIsActive; }
    activity::Type getActivityType() const { return mActivityType; }
    FunctionalityRequirement getFunctionalityRequirement() const { return mFunctionalityRequirement; }

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
