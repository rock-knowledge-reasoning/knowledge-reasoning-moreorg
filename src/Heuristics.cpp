#include "Heuristics.hpp"
#include "facets/Robot.hpp"

namespace organization_model {

Heuristics::Heuristics(const OrganizationModelAsk& ask)
    : mAsk(ask)
{
}

base::Position Heuristics::positionLinear(const Agent& agent,
        const base::Position& from,
        const base::Position& to,
        size_t progressedTime) const
{
    ModelPool m = agent.getType();
    facets::Robot robot(m, mAsk);

    double nominalVelocity = robot.getNominalVelocity();

    // s = v*t
    base::Vector3d direction = to - from;
    // maximum travelled distance is the direct linear distance
    double distanceTravelled = std::min(nominalVelocity*progressedTime, direction.norm());
    base::Position current = from + direction*(distanceTravelled/direction.norm());

    return current;
}

double Heuristics::travelTime(const Agent& agent,
        const base::Position& from,
        const base::Position& to) const
{
    ModelPool m = agent.getType();
    facets::Robot robot(m, mAsk);

    double nominalVelocity = robot.getNominalVelocity();
    // s = v*t
    base::Vector3d direction = to - from;
    return direction.norm()/nominalVelocity;
}

double Heuristics::travelTime(const StatusSample* sample) const
{
    StatusSample::RawPtr2Double::const_iterator cit = mTravelTime.find(sample);
    if(cit != mTravelTime.end())
    {
        return cit->second;
    }

    double time = travelTime(sample->getAgent(),
            sample->getFromLocation(),
            sample->getToLocation());
    mTravelTime[sample] = time;
    return time;
}

double Heuristics::waitTime(const Agent& agent,
    const base::Position& from,
    const base::Position& to,
    size_t availableTime) const
{
    return availableTime - travelTime(agent, from, to);
}

double Heuristics::waitTime(const StatusSample* sample) const
{
    return sample->getAvailableTime() - travelTime(sample);
}

double Heuristics::getEnergyConsumption(const StatusSample* sample) const
{
    StatusSample::RawPtr2Double::const_iterator cit = mEnergyConsumption.find(sample);
    if(cit != mEnergyConsumption.end())
    {
        return cit->second;
    }

    double requiredTravelTime = travelTime(sample);
    double requiredWaitTime = waitTime(sample);

    double energyConsumption = sample->getAgent().getFacet(mAsk).estimatedEnergyCostFromTime(requiredTravelTime + requiredWaitTime);

    mEnergyConsumption[sample] = energyConsumption;
    return energyConsumption;
}

double Heuristics::getEnergyReductionAbsolute(const StatusSample* sample,
        const AtomicAgent& atomicAgent,
        size_t fromTime,
        size_t toTime) const
{
    try {
        double consumption = getEnergyConsumption(sample);
        double share = sample->getAgent().getEnergyProviderShares().at(atomicAgent);

        // use linear model to estimate consumption
        return consumption*share* (toTime - fromTime)/sample->getAvailableTime();
    } catch(const std::out_of_range& e)
    {
        throw std::runtime_error("organization_model::Heuristics::getEnergyReductionAbsolute: "
                " please call update function on 'agent' to prepare information about energy provider shares");
    }
}

} // end namespace organization_model
