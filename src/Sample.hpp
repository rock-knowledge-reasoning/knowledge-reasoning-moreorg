#ifndef ORGANIZATION_MODEL_SAMPLE_HPP
#define ORGANIZATION_MODEL_SAMPLE_HPP

#include <base/Pose.hpp>

namespace moreorg {

class Sample
{
public:
    Sample(const base::Position& fromLocation,
           const base::Position& toLocation,
           size_t fromTime,
           size_t toTime
    );

    virtual ~Sample();

    const base::Position& getFromLocation() const { return mFromLocation; }
    const base::Position& getToLocation() const { return mToLocation; }

    size_t getFromTime() const { return mFromTime; }
    size_t getToTime() const { return mToTime; }
    size_t getAvailableTime() const { return mToTime - mFromTime; }

    /**
     * Check if the status sample intersects with the given time
     * \return true if \p time lies in the interval of getFromTime and
     * getToTime
     */
    bool matchesTime(size_t time) const { return time >= mFromTime && time <= mToTime; }

    bool sameLocation(const Sample& other) const { return mFromLocation == other.mFromLocation &&
        mToLocation == other.mToLocation; }

protected:
    base::Position mFromLocation;
    base::Position mToLocation;
    size_t mFromTime;
    size_t mToTime;
};

};
#endif // ORGANIZATION_MODEL_SAMPLE_HPP
