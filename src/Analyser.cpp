#include "Analyser.hpp"
#include <algorithm>

namespace organization_model {

Analyser::Analyser(const OrganizationModelAsk& ask)
    : mAsk(ask)
    , mHeuristics(ask)
    , mMinTime(std::numeric_limits<size_t>::max())
    , mMaxTime(std::numeric_limits<size_t>::min())
{
}


void Analyser::add(const StatusSample& sample)
{
    mSamples.push_back(sample);

    mMinTime = std::min(mMinTime, sample.getFromTime());
    mMaxTime = std::max(mMaxTime, sample.getToTime());
}

void Analyser::createIndex()
{
    // Index sample by time
    mTimeIndexedSamples = SampleIndex(mMaxTime+1);
    for(const StatusSample& statusSample : mSamples)
    {
        for(size_t i = statusSample.getFromTime(); i <= statusSample.getToTime(); ++i)
        {
            mTimeIndexedSamples[i].push_back(&statusSample);
        }

        for(const AtomicAgent& agent : statusSample.getAgent().getAtomicAgents())
        {
            mAtomicAgentIndexedSamples[agent].push_back(&statusSample);
        }
    }
}

StatusSample::ConstRawPtrList Analyser::samplesAt(size_t time) const
{
    return mTimeIndexedSamples[time];
}

StatusSample::ConstRawPtrList Analyser::samplesFor(const AtomicAgent& atomicAgent) const
{
    std::map<AtomicAgent, StatusSample::ConstRawPtrList>::const_iterator cit = mAtomicAgentIndexedSamples.find(atomicAgent);
    if(cit != mAtomicAgentIndexedSamples.end())
    {
        return cit->second;
    }
    throw std::invalid_argument("organization_model::Analyser::samplesFor: the given atomic agent is not unknown");
}

Agent::List Analyser::getCoalitionStructure(size_t time) const
{
    Agent::List agentList;
    StatusSample::ConstRawPtrList samples = samplesAt(time);
    for(const StatusSample* sample : samples)
    {
        agentList.push_back(sample->getAgent());
    }
    return agentList;
}

std::vector<base::Position> Analyser::getPositions(size_t time) const
{
    std::vector<base::Position> positions;
    StatusSample::ConstRawPtrList samples = samplesAt(time);
    for(const StatusSample* sample : samples)
    {
        base::Position position = mHeuristics.positionLinear(sample->getAgent(),
                sample->getFromLocation(),
                sample->getToLocation(),
                time - sample->getFromTime());
        positions.push_back(position);
    }
    return positions;
}

std::vector<activity::Type> Analyser::getActivities(size_t time) const
{
    std::vector<activity::Type> activities;
    StatusSample::ConstRawPtrList samples = samplesAt(time);
    for(const StatusSample* sample : samples)
    {
        activities.push_back(sample->getActivityType());
    }
    return activities;
}


double Analyser::getEnergyReductionAbsolute(const StatusSample* sample, const AtomicAgent& atomicAgent, size_t fromTime, size_t toTime) const
{
    return mHeuristics.getEnergyReductionAbsolute(sample, atomicAgent, fromTime, toTime);
}

double Analyser::getEnergyReductionAbsolute(size_t time, const AtomicAgent& atomicAgent) const
{
    double energy = 0;
    StatusSample::ConstRawPtrList samples = samplesFor(atomicAgent);
    for(const StatusSample* sample : samples)
    {
        // stop with get from time
        if(sample->getFromTime() <= time && sample->getToTime() >= time)
        {
            energy += getEnergyReductionAbsolute(sample, atomicAgent, sample->getFromTime(),time);
            // last sample which needs to be partially evaluated
            break;
        } else {
            // we assume a linear distribution of the cost
            // (a) cost to move from a --> b [assume main transport system as
            // cost driver]
            // (b) cost to perform function
            energy += getEnergyReductionAbsolute(sample, atomicAgent, sample->getFromTime(), sample->getToTime());
        }
    }
    return energy;
}

double Analyser::getEnergyReductionAbsolute(size_t time, const Agent& agent) const
{
    double energyReduction = 0;
    for(const AtomicAgent& aa : agent.getAtomicAgents())
    {
        energyReduction += getEnergyReductionAbsolute(time, aa);
    }
    return energyReduction;
}

std::vector<double> Analyser::getEnergyReductionAbsolute(size_t time) const
{
    std::vector<double> energyReductionAbsolute;
    Agent::List agents = getCoalitionStructure(time);
    for(const Agent& agent : agents)
    {
        double energyReduction = getEnergyReductionAbsolute(time, agent);
        energyReductionAbsolute.push_back(energyReduction);
    }
    return energyReductionAbsolute;
}

double Analyser::getEnergyAvailableAbsolute(size_t time, const Agent& agent) const
{
    double fullCapacity = agent.getFacet(mAsk).getEnergyCapacity();
    return fullCapacity - getEnergyReductionAbsolute(time, agent);
}

std::vector<double> Analyser::getEnergyAvailableAbsolute(size_t time) const
{
    std::vector<double> energyAvailableAbsolute;
    Agent::List agents = getCoalitionStructure(time);
    for(const Agent& agent : agents)
    {
        double energyAvailable = getEnergyAvailableAbsolute(time, agent);
        energyAvailableAbsolute.push_back(energyAvailable);
    }
    return energyAvailableAbsolute;
}

double Analyser::getEnergyReductionRelative(size_t time, const AtomicAgent& atomicAgent) const
{
    double fullEnergyCapacity = atomicAgent.getFacet(mAsk).getEnergyCapacity();
    double energyReduction  = getEnergyReductionAbsolute(time, atomicAgent);

    if(energyReduction == 0)
    {
        return 0;
    } else {
        return energyReduction/fullEnergyCapacity;
    }
}

double Analyser::getEnergyReductionRelative(size_t time, const Agent& agent) const
{
    double fullEnergyCapacity = agent.getFacet(mAsk).getEnergyCapacity();
    return getEnergyReductionAbsolute(time, agent) / fullEnergyCapacity;
}

double Analyser::getEnergyAvailableRelative(size_t time, const Agent& agent) const
{
    return 1 - getEnergyReductionRelative(time, agent);
}

std::vector<double> Analyser::getEnergyAvailableRelative(size_t time) const
{
    std::vector<double> energyAvailableRelative;
    Agent::List agents = getCoalitionStructure(time);
    for(const Agent& agent : agents)
    {
        double energyAvailable = getEnergyAvailableRelative(time, agent);
        energyAvailableRelative.push_back(energyAvailable);
    }
    return energyAvailableRelative;
}


double Analyser::getMedian(std::vector<double> values)
{
    size_t valueCount = values.size();
    if(valueCount%2 == 0)
    {
        size_t pos0 = valueCount/2;
        std::nth_element(values.begin(), values.begin() + pos0, values.end());
        double val0 = values[pos0];

        size_t pos1 = pos0 + 1;
        std::nth_element(values.begin(), values.begin() + pos1, values.end());
        double val1 = values[pos1];

        return 0.5* (val0 + val1);
    } else {
        size_t pos0 = (valueCount+1)/2;
        std::nth_element(values.begin(), values.begin() + pos0, values.end());
        return values[pos0];
    }
}

double Analyser::getEnergyMAD(size_t time) const
{
    std::vector<double> energyAvailable = getEnergyAvailableRelative(time);
    double median = Analyser::getMedian(energyAvailable);

    std::vector<double> deltas;
    for(double value : energyAvailable)
    {
        deltas.push_back( fabs(value - median) );
    }
    return Analyser::getMedian(deltas);
}

double Analyser::getEnergyAvailableMinRelative(size_t time) const
{
    std::vector<double> energyAvailable = getEnergyAvailableRelative(time);
    double min = std::numeric_limits<double>::max();
    for(double v : energyAvailable)
    {
        if(v < min)
        {
            min = v;
        }
    }
    return min;
}

double Analyser::getTravelDistance(size_t time, const AtomicAgent& atomicAgent) const
{
    double distance = 0;
    const StatusSample::ConstRawPtrList& samples = samplesFor(atomicAgent);
    for(const StatusSample* sample : samples)
    {
        // stop with get from time
        if(sample->getFromTime() <= time && sample->getToTime() >= time)
        {
            distance += (sample->getToLocation() - sample->getFromLocation()).norm()*(time - sample->getFromTime());
            // last sample which needs to be partially evaluated
            break;
        } else {
            distance += (sample->getToLocation() - sample->getFromLocation()).norm();
        }
    }
    return distance;
}
        distance += (sample->getToLocation() - sample->getFromLocation()).norm();
    }
    return distance;
}

double Analyser::getOperativeTime(size_t time, const AtomicAgent& atomicAgent) const
{
    throw std::runtime_error("Not implemented");
}

double Analyser::getDormantTime(size_t time, const AtomicAgent& atomicAgent) const
{
    throw std::runtime_error("Not implemented");
}

} // end namespace organization_model
