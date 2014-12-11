#include "OWLRestriction.hpp"
#include <stdexcept>
#include <sstream>
#include <ostream>

namespace owlapi {
namespace model {

std::string OWLRestriction::toString() const
{
    std::stringstream ss;
    ss << "OWLRestriction";
    if(isDataRestriction())
    {
        ss << " (Data):" << std::endl;
    } else if(isObjectRestriction())
    {
        ss << " (Object):" << std::endl;
    } else {
        throw std::runtime_error("OWLRestriction: neither data nor object restriction -- internal error");
    }
    ss << "    property: " << getProperty()->toString() << std::endl;
    return ss.str();
}

} // end namespace model
} // end namespace owlapi

std::ostream& operator<<(std::ostream& os, const owlapi::model::OWLRestrictionPtrList& list)
{
    owlapi::model::OWLRestrictionPtrList::const_iterator cit = list.begin();
    for(; cit != list.end(); ++cit)
    {
        if(*cit)
        {
            os << (*cit)->toString() << std::endl;
        }
    }
    return os;
}
