#include "Analyser.hpp"

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
    mSampleIndex = SampleIndex(mMaxTime+1);
    for(const StatusSample& statusSample : mSamples)
    {
        for(size_t i = statusSample.getFromTime(); i <= statusSample.getToTime(); ++i)
        {
            mSampleIndex[i].push_back(&statusSample);
        }
    }
}

StatusSample::ConstRawPtrList Analyser::samplesAt(size_t time)
{
    return mSampleIndex[time];
}

Agent::List Analyser::getCoalitionStructure(size_t time)
{
    Agent::List agentList;
    StatusSample::ConstRawPtrList samples = samplesAt(time);
    for(const StatusSample* sample : samples)
    {
        agentList.push_back(sample->getAgent());
    }
    return agentList;
}

std::vector<base::Position> Analyser::getPositions(size_t time)
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

} // end namespace organization_model
