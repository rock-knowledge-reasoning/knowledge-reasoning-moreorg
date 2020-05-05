#ifndef ORGANIZATION_MODEL_POLICY_HPP
#define ORGANIZATION_MODEL_POLICY_HPP

#include "OrganizationModelAsk.hpp"

namespace moreorg {

class Policy
{
public:
    Policy();

    Policy(const ModelPool& pool, const OrganizationModelAsk& ask);

    virtual ~Policy();

    /**
     * Update the policy object
     */
    virtual void update(const ModelPool& pool, const OrganizationModelAsk& ask) = 0;

protected:
    ModelPool mModelPool;
    OrganizationModelAsk mAsk;

};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_POLICY_HPP
