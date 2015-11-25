#include "Service.hpp"

namespace organization_model {

bool Service::operator<(const Service& other) const
{
    return this->getModel() < other.getModel();
}

} // end namespace organization_model
