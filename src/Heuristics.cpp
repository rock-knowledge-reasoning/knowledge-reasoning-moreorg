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
        size_t progressedTime)
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
        const base::Position& to)
{
    ModelPool m = agent.getType();
    facets::Robot robot(m, mAsk);

    double nominalVelocity = robot.getNominalVelocity();
    // s = v*t
    base::Vector3d direction = to - from;
    return direction.norm()/nominalVelocity;
}

double Heuristics::waitTime(const Agent& agent,
    const base::Position& from,
    const base::Position& to,
    size_t availableTime)
{
    return availableTime - travelTime(agent, from, to);
}


} // end namespace organization_model
