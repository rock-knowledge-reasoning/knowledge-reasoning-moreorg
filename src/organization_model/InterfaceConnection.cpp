#include "InterfaceConnection.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdint.h>

using namespace owlapi::model;

namespace moreorg {
namespace moreorg {

InterfaceConnection::InterfaceConnection() {}
InterfaceConnection::InterfaceConnection(const IRI& interface0,
                                         const IRI& interface1)
    : begin(interface0)
    , end(interface1)
{
}

void InterfaceConnection::addParent(const IRI& parent)
{
    parents.push_back(parent);
    if(parents.size() >= 2)
    {
        std::sort(parents.begin(), parents.end());
    }
}

bool InterfaceConnection::sameParents(const InterfaceConnection& other) const
{
    return other.parents == this->parents;
}

bool InterfaceConnection::useSameInterface(
    const InterfaceConnection& other) const
{
    return other.begin == this->begin || other.begin == this->end ||
           other.end == this->begin || other.end == this->end;
}

bool InterfaceConnection::selfReferencing() const
{
    return parents.size() == 2 && parents[0] != parents[1];
}

bool InterfaceConnection::operator<(const InterfaceConnection& other) const
{
    if(begin < other.begin)
    {
        return true;
    } else if(begin == other.begin)
    {
        if(end < other.end)
        {
            return true;
        }
    }
    return false;
}

bool InterfaceConnection::operator==(const InterfaceConnection& other) const
{
    return end == other.end && begin == other.begin;
}

std::string InterfaceConnection::toString() const
{
    // return "InterfaceConnection from: " + begin.getFragment() + " to: " +
    // end.getFragment() + "\n        parents: " + parents[0].getFragment() + "
    // to " + parents[1].getFragment();
    std::stringstream ss;
    ss << "InterfaceConnection from: " << begin.toString()
       << " to: " << end.toString();
    // ss << std::endl << "        parents: " << parents[0].getFragment() << "
    // to " << parents[1].getFragment();
    return ss.str();
}

std::ostream& operator<<(std::ostream& os,
                         const InterfaceConnection& connection)
{
    os << connection.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const InterfaceConnectionList& list)
{
    InterfaceConnectionList::const_iterator cit = list.begin();
    os << "[" << std::endl;
    for(; cit != list.end(); ++cit)
    {
        os << *cit;
        if(cit + 1 != list.end())
        {
            os << "," << std::endl;
        }
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const InterfaceCombinationList& list)
{
    InterfaceCombinationList::const_iterator cit = list.begin();
    os << "[" << std::endl;
    uint32_t count = 0;
    for(; cit != list.end(); ++cit)
    {
        os << "#" << ++count << ": " << *cit << std::endl;
    }
    os << "]" << std::endl;
    return os;
}

} // end namespace moreorg
} // end namespace moreorg
