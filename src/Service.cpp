#include "Service.hpp"

namespace moreorg {

bool Service::operator<(const Service& other) const
{
    return this->getModel() < other.getModel();
}

} // end namespace moreorg
