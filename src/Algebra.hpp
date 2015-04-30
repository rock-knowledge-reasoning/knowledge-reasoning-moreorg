#ifndef ORGANIZATION_MODEL_ALGEBRA_HPP
#define ORGANIZATION_MODEL_ALGEBRA_HPP

#include <organization_model/OrganizationModel.hpp>

namespace organization_model {

class Algebra
{
public:
    static ModelPoolDelta delta(const ModelPoolDelta& a, const ModelPoolDelta& b);
    static ModelPoolDelta sum(const ModelPoolDelta& a, const ModelPoolDelta& b);
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_HPP
