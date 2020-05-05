#include "Policy.hpp"

namespace moreorg {

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

} // end namespace moreorg
