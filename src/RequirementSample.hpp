#ifndef ORGANIZATION_MODEL_REQUIREMENT_SAMPLE_HPP
#define ORGANIZATION_MODEL_REQUIREMENT_SAMPLE_HPP

#include "ModelPool.hpp"
#include "Resource.hpp"
#include "Sample.hpp"

namespace moreorg {

/**
 * A requirement associate with location and time
 */
class RequirementSample : public Sample
{
public:
    using List = std::vector<RequirementSample>;
    using ConstRawPtrList = std::vector<const RequirementSample*>;

    RequirementSample(const Resource::Set& resources,
                      const ModelPool& agentPool,
                      const base::Position& fromLocation,
                      const base::Position& toLocation,
                      size_t fromTime,
                      size_t toTime);

    virtual ~RequirementSample();

    /**
     * Get resource requirements
     * \return resource requirements
     */
    const Resource::Set& getResources() const { return mResources; }

    /**
     * The agent pool minimum requirements
     * \return agent pool requirements
     */
    const ModelPool& getAgentPool() const { return mAgentPool; }

protected:
    Resource::Set mResources;
    ModelPool mAgentPool;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_REQUIREMENT_SAMPLE_HPP
