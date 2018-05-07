#include "Analyser.hpp"
#include <algorithm>
#include <functional>

using namespace std::placeholders;

namespace organization_model {

std::map<Analyser::AtomicAgentSampleType, std::string> Analyser::AtomicAgentSampleTypeTxt =
{
    { AA_EnergyReductionAbsolute, "EnergyReductionAbsolute" },
    { AA_EnergyReductionRelative, "EnergyReductionRelative" },
    { AA_TravelDistance,          "TravelDistance" },
    { AA_TravelDistanceToNextPOI, "TravelDistanceToNextPOI" },
    { AA_OperativeTime,           "OperativeTime" },
    { AA_DormantTime,             "DormantTime" },
    { AA_Redundancy,              "Redundancy" }
};

std::map<Analyser::AgentSampleType, std::string> Analyser::AgentSampleTypeTxt =
{
    { A_EnergyReductionAbsolute, "EnergyReductionAbsolute" },
    { A_EnergyReductionRelative, "EnergyReductionRelative" },
    { A_EnergyAvailableAbsolute, "EnergyAvailableAbsolute" },
    { A_EnergyAvailableRelative, "EnergyAvailableRelative" },
    { A_Redundancy,              "Redundancy" }
};

std::map<Analyser::OrganizationSampleType, std::string> Analyser::OrganizationSampleTypeTxt =
{
    { O_EnergyMedianAbsoluteDeviation, "EnergyMedianAbsoluteDeviation" },
    { O_EnergyAvailableMinRelative,    "EnergyAvailableMinRelative" },
    { O_RedundancyMin,                 "RedundancyMin" },
    { O_RedundancyMax,                 "RedundancyMax" },
};

Analyser::Analyser(const OrganizationModelAsk& ask)
    : mAsk(ask)
    , mHeuristics(ask)
    , mStatusMinTime(std::numeric_limits<size_t>::max())
    , mStatusMaxTime(std::numeric_limits<size_t>::min())
    , mRequirementMinTime(std::numeric_limits<size_t>::max())
    , mRequirementMaxTime(std::numeric_limits<size_t>::min())
{
    // Add the sample function map for the atomic agent
    mAtomicAgentSampleFunctionMap =
    {
        { AA_EnergyReductionAbsolute, std::bind(&Analyser::getEnergyReductionAbsoluteAA,this, _1,_2)},
        { AA_EnergyReductionRelative, std::bind(&Analyser::getEnergyReductionRelativeAA,this, _1,_2)},
        { AA_TravelDistance, std::bind(&Analyser::getTravelDistanceAA,this, _1,_2)},
        { AA_TravelDistanceToNextPOI, std::bind(&Analyser::getTravelDistanceToNextPOIAA,this, _1,_2)},
        { AA_OperativeTime, std::bind(&Analyser::getOperativeTimeAA,this, _1,_2)},
        { AA_DormantTime, std::bind(&Analyser::getDormantTimeAA,this, _1,_2)}
    };
    setSampleColumnsAA(std::vector<AtomicAgentSampleType>());

    // Add the sample function map for agent
    mAgentSampleFunctionMap =
    {
        { A_EnergyReductionAbsolute, std::bind(&Analyser::getEnergyReductionAbsoluteA,this, _1,_2)},
        { A_EnergyReductionRelative, std::bind(&Analyser::getEnergyReductionRelativeA,this, _1,_2)},
        { A_EnergyAvailableAbsolute, std::bind(&Analyser::getEnergyAvailableAbsoluteA,this, _1,_2)},
        { A_EnergyAvailableRelative, std::bind(&Analyser::getEnergyAvailableRelativeA,this, _1,_2)},
    };
    setSampleColumnsA(std::vector<AgentSampleType>());

    mOrganizationSampleFunctionMap =
    {
        { O_EnergyAvailableMinRelative, std::bind(&Analyser::getEnergyAvailableMinRelative,this, _1)},
        { O_EnergyMedianAbsoluteDeviation, std::bind(&Analyser::getEnergyMAD,this, _1)},
        { O_RedundancyMin, std::bind(&Analyser::getMinRedundancy,this, _1) },
        { O_RedundancyMax, std::bind(&Analyser::getMaxRedundancy,this, _1) },
    };
    setSampleColumnsO(std::vector<OrganizationSampleType>());
}


void Analyser::add(const StatusSample& sample)
{
    mStatusSamples.push_back(sample);

    mStatusMinTime = std::min(mStatusMinTime, sample.getFromTime());
    mStatusMaxTime = std::max(mStatusMaxTime, sample.getToTime());
}

void Analyser::add(const RequirementSample& sample)
{
    mRequirementSamples.push_back(sample);

    mRequirementMinTime = std::min(mRequirementMinTime, sample.getFromTime());
    mRequirementMaxTime = std::max(mRequirementMaxTime, sample.getToTime());
}

void Analyser::createIndex()
{
    // Index sample by time
    mTimeIndexedStatusSamples = StatusSampleIndex(mStatusMaxTime+1);
    for(const StatusSample& statusSample : mStatusSamples)
    {
        for(size_t i = statusSample.getFromTime(); i <= statusSample.getToTime(); ++i)
        {
            mTimeIndexedStatusSamples[i].push_back(&statusSample);
        }

        for(const AtomicAgent& agent : statusSample.getAgent().getAtomicAgents())
        {
            mAtomicAgentIndexedSamples[agent].push_back(&statusSample);
        }
    }

    mTimeIndexedRequirementSamples = RequirementSampleIndex(mStatusMaxTime+1);
    for(const RequirementSample& sample : mRequirementSamples)
    {
        for(size_t i = sample.getFromTime(); i <= sample.getToTime(); ++i)
        {
            mTimeIndexedRequirementSamples[i].push_back(&sample);
        }
    }
}

StatusSample::ConstRawPtrList Analyser::statusSamplesAt(size_t time) const
{
    return mTimeIndexedStatusSamples[time];
}

RequirementSample::ConstRawPtrList Analyser::requirementSamplesAt(size_t time) const
{
    return mTimeIndexedRequirementSamples[time];
}

StatusSample::ConstRawPtrList Analyser::statusSamplesFor(const AtomicAgent& atomicAgent) const
{
    std::map<AtomicAgent, StatusSample::ConstRawPtrList>::const_iterator cit = mAtomicAgentIndexedSamples.find(atomicAgent);
    if(cit != mAtomicAgentIndexedSamples.end())
    {
        return cit->second;
    }
    throw std::invalid_argument("organization_model::Analyser::statusSamplesFor: the given atomic agent is not unknown");
}

Agent::List Analyser::getCoalitionStructure(size_t time) const
{
    Agent::List agentList;
    StatusSample::ConstRawPtrList samples = statusSamplesAt(time);
    for(const StatusSample* sample : samples)
    {
        agentList.push_back(sample->getAgent());
    }
    return agentList;
}

std::vector<base::Position> Analyser::getPositions(size_t time) const
{
    std::vector<base::Position> positions;
    StatusSample::ConstRawPtrList samples = statusSamplesAt(time);
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

std::vector<Resource::Set> Analyser::getRequirements(size_t time) const
{
    std::vector<Resource::Set> resourceRequirements;

    StatusSample::ConstRawPtrList statusSamples = statusSamplesAt(time);
    RequirementSample::ConstRawPtrList requirementSamples = requirementSamplesAt(time);

    for(const StatusSample* statusSample : statusSamples)
    {
        Resource::Set resources;
        for(const RequirementSample* requirementSample : requirementSamples)
        {
            if(statusSample->sameLocation(*requirementSample))
            {
                resources = Resource::merge(resources, requirementSample->getResources());
            }
        }
        resourceRequirements.push_back(resources);
    }
    return resourceRequirements;
}

std::vector<activity::Type> Analyser::getActivities(size_t time) const
{
    std::vector<activity::Type> activities;
    StatusSample::ConstRawPtrList samples = statusSamplesAt(time);
    for(const StatusSample* sample : samples)
    {
        activities.push_back(sample->getActivityType());
    }
    return activities;
}

void Analyser::setSampleColumnsAA(const std::vector<AtomicAgentSampleType>& columnTypes)
{
    mAtomicAgentSampleTypes.clear();
    mAtomicAgentSampleFunctions.clear();
    for(const std::pair<AtomicAgentSampleType, AtomicAgentSampleFunc>& p : mAtomicAgentSampleFunctionMap)
    {
        if(columnTypes.empty() || columnTypes.end() != std::find(columnTypes.begin(), columnTypes.end(), p.first))
        {
            mAtomicAgentSampleTypes.push_back(p.first);
            mAtomicAgentSampleFunctions.push_back(p.second);
        }
    }
}

std::string Analyser::getSampleColumnDescriptionAA() const
{
    std::stringstream ss;
    for(AtomicAgentSampleType t : mAtomicAgentSampleTypes)
    {
        ss << AtomicAgentSampleTypeTxt[t];
        ss << " ";

    }
    std::string description = ss.str();
    if(!description.empty())
    {
        description.erase(description.size() - 1);
    }
    return description;
}

std::vector<double> Analyser::getSampleAA(size_t time, const AtomicAgent& atomicAgent) const
{
    std::vector<double> row;
    for(AtomicAgentSampleFunc f : mAtomicAgentSampleFunctions)
    {
        row.push_back( f(time, atomicAgent) );
    }
    return row;
}

void Analyser::setSampleColumnsA(const std::vector<AgentSampleType>& columnTypes)
{
    mAgentSampleTypes.clear();
    mAgentSampleFunctions.clear();
    for(const std::pair<AgentSampleType, AgentSampleFunc>& p : mAgentSampleFunctionMap)
    {
        if(columnTypes.empty() || columnTypes.end() != std::find(columnTypes.begin(), columnTypes.end(), p.first))
        {
            mAgentSampleTypes.push_back(p.first);
            mAgentSampleFunctions.push_back(p.second);
        }
    }
}

std::string Analyser::getSampleColumnDescriptionA() const
{
    std::stringstream ss;
    for(AgentSampleType t : mAgentSampleTypes)
    {
        ss << AgentSampleTypeTxt[t];
        ss << " ";

    }
    std::string description = ss.str();
    if(!description.empty())
    {
        description.erase(description.size() -1);
    }
    return description;
}

std::vector<double> Analyser::getSampleA(size_t time, const Agent& agent) const
{
    std::vector<double> row;
    for(AgentSampleFunc f : mAgentSampleFunctions)
    {
        row.push_back( f(time, agent) );
    }
    return row;
}

void Analyser::setSampleColumnsO(const std::vector<OrganizationSampleType>& columnTypes)
{
    mOrganizationSampleTypes.clear();
    mOrganizationSampleFunctions.clear();
    for(const std::pair<OrganizationSampleType, OrganizationSampleFunc>& p : mOrganizationSampleFunctionMap)
    {
        if(columnTypes.empty() || columnTypes.end() != std::find(columnTypes.begin(), columnTypes.end(), p.first))
        {
            mOrganizationSampleTypes.push_back(p.first);
            mOrganizationSampleFunctions.push_back(p.second);
        }
    }
}

std::string Analyser::getSampleColumnDescriptionO() const
{
    std::stringstream ss;
    for(OrganizationSampleType t : mOrganizationSampleTypes)
    {
        ss << OrganizationSampleTypeTxt[t];
        ss << " ";

    }
    std::string description = ss.str();
    if(!description.empty())
    {
        description.erase(description.size() - 1);
    }
    return description;
}
std::vector<double> Analyser::getSampleO(size_t time) const
{
    std::vector<double> row;
    for(OrganizationSampleFunc f : mOrganizationSampleFunctions)
    {
        row.push_back( f(time) );
    }
    return row;
}

double Analyser::getEnergyReductionAbsolute(const StatusSample* sample, const AtomicAgent& atomicAgent, size_t fromTime, size_t toTime) const
{
    return mHeuristics.getEnergyReductionAbsolute(sample, atomicAgent, fromTime, toTime);
}

double Analyser::getEnergyReductionAbsolute(size_t time, const AtomicAgent& atomicAgent) const
{
    double energy = 0;
    StatusSample::ConstRawPtrList samples = statusSamplesFor(atomicAgent);
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

double Analyser::getEnergyAvailableAbsolute(size_t time, const Agent& agent) const
{
    double fullCapacity = agent.getFacade(mAsk).getEnergyCapacity();
    return fullCapacity - getEnergyReductionAbsolute(time, agent);
}

std::vector<double> Analyser::getCoalitionStructureValue(size_t time, AgentSampleType sampleType) const
{
    std::vector<double> values;
    Agent::List agents = getCoalitionStructure(time);
    std::map<AgentSampleType, AgentSampleFunc>::const_iterator it = mAgentSampleFunctionMap.find(sampleType);
    if(it == mAgentSampleFunctionMap.end())
    {
        throw std::invalid_argument("organization_model::Analyser::getCoalitionStructureValue: not function registered for sample"
                " type: '" + AgentSampleTypeTxt[sampleType]  + "'");
    }

    AgentSampleFunc func = it->second;
    for(const Agent& agent : agents)
    {
        values.push_back( func(time, agent) );
    }
    return values;
}

double Analyser::getEnergyReductionRelative(size_t time, const AtomicAgent& atomicAgent) const
{
    double fullEnergyCapacity = atomicAgent.getFacade(mAsk).getEnergyCapacity();
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
    double fullEnergyCapacity = agent.getFacade(mAsk).getEnergyCapacity();
    return getEnergyReductionAbsolute(time, agent) / fullEnergyCapacity;
}

double Analyser::getEnergyAvailableRelative(size_t time, const Agent& agent) const
{
    return 1 - getEnergyReductionRelative(time, agent);
}

std::vector<double> Analyser::getRedundancy(size_t time) const
{
    return getMetric(time, metrics::REDUNDANCY);
}

std::vector<double> Analyser::getMetric(size_t time, metrics::Type metricType) const
{
    std::vector<double> values;

    Metric::Ptr metric = Metric::getInstance(metricType, mAsk);

    Agent::List agents = getCoalitionStructure(time);
    std::vector<Resource::Set> requirements = getRequirements(time);

    for(size_t i = 0; i < agents.size(); ++i)
    {
        const Agent& agent = agents[i];
        const Resource::Set& requirementSet = requirements[i];

        owlapi::model::IRISet functionSet = Resource::getModels(requirementSet);
        owlapi::model::IRIList functions(functionSet.begin(), functionSet.end());

        // metrics need to explicitly deal with empty functional requirement
        double val = metric->computeSequential(functions, agent.getType());
        values.push_back(val);
    }
    return values;
}


double Analyser::getMedian(std::vector<double> values)
{
    if(values.empty())
    {
        throw std::invalid_argument("organization_model::Analyser::getMedian: no values provided");
    }

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

numeric::Stats<double> Analyser::getStats(const std::vector<double>& values)
{
    if(values.empty())
    {
        throw std::invalid_argument("organization_model::Analyser::getStats: no values provided");
    }
    numeric::Stats<double> stats;
    for(double v : values)
    {
        stats.update(v);
    }
    return stats;
}

double Analyser::getMedianAbsoluteDeviation(const std::vector<double>& values)
{
    if(values.empty())
    {
        throw std::invalid_argument("organization_model::Analyser::getMedianAbsoluteDeviation: no values provided");
    }

    double median = Analyser::getMedian(values);
    std::vector<double> deltas;
    for(double value : values)
    {
        deltas.push_back( fabs(value - median) );
    }
    return Analyser::getMedian(deltas);
}

double Analyser::getTravelDistance(size_t time, const AtomicAgent& atomicAgent) const
{
    double distance = 0;
    const StatusSample::ConstRawPtrList& samples = statusSamplesFor(atomicAgent);
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

double Analyser::getTravelDistance(const AtomicAgent& atomicAgent) const
{
    double distance = 0;
    const StatusSample::ConstRawPtrList& samples = statusSamplesFor(atomicAgent);
    for(const StatusSample* sample : samples)
    {
        distance += (sample->getToLocation() - sample->getFromLocation()).norm();
    }
    return distance;
}

double Analyser::getTravelDistanceToNextPOI(size_t time, const AtomicAgent& atomicAgent) const
{
    const StatusSample::ConstRawPtrList& samples = statusSamplesFor(atomicAgent);
    for(const StatusSample* sample : samples)
    {
        // stop with get from time
        if(sample->getFromTime() <= time && sample->getToTime() >= time)
        {
            // distance to next POI is compute based on the assumption of a
            // linear approach function
            return (sample->getToLocation() - sample->getFromLocation()).norm()*(time/(sample->getToTime() - sample->getFromTime()) );
            // last sample which needs to be partially evaluated
        }
    }
    return 0;
}

double Analyser::getOperativeTime(size_t time, const AtomicAgent& atomicAgent) const
{
    throw std::runtime_error("Not implemented");
}

double Analyser::getDormantTime(size_t time, const AtomicAgent& atomicAgent) const
{
    throw std::runtime_error("Not implemented");
}

std::string Analyser::toString(const std::vector<double>& data)
{
    std::stringstream ss;
    for(double v : data)
    {
        ss << v;
        ss << " ";
    }
    std::string row = ss.str();
    row.erase(row.end(), row.end());
    return row;
}

} // end namespace organization_model
