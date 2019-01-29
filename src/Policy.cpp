#include "Policy.hpp"

namespace organization_model {

Policy::Policy()
    : mModelPool()
    , mAsk()
{}

Policy::Policy(const ModelPool& pool, const OrganizationModelAsk& ask)
    : mModelPool(pool)
    , mAsk(ask)
{
}

Policy::~Policy()
{
}

} // end namespace organization_model
