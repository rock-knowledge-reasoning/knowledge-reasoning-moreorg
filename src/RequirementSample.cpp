#include "RequirementSample.hpp"

namespace organization_model {

RequirementSample::RequirementSample(const Resource::Set& resources,
       const ModelPool& agentPool,
       const base::Position& fromLocation,
       const base::Position& toLocation,
       size_t fromTime,
       size_t toTime)
    : Sample(fromLocation, toLocation, fromTime, toTime)
    , mResources(resources)
    , mAgentPool(agentPool)
{
}

RequirementSample::~RequirementSample()
{}

}
