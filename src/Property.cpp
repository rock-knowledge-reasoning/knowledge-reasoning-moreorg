#include "Property.hpp"
#include <sstream>

namespace owl_om {

void Property::addInverseOf(const Name& name)
{
    NameList::const_iterator cit = std::find(mInverseOf.begin(), mInverseOf.end(), name);
    if(cit != mInverseOf.end())
    {
        mInverseOf.push_back(cit);
    }
}

std::string Property::toString() const
{
    std::stringstream txt;
    txt << type() << ": " << getName() << std::endl;
    txt << "    superclasses: " << std::endl;
    Class::Map::const_iterator sit = mSuperClasses.begin();
    for(; sit != mSuperClasses.end(); ++sit)
    {
        txt << "        " << sit->first << std::endl;
    }
    txt << "    domain: " << mDomain << std::endl;
    txt << "    range: " << mRange << std::endl;
    text<< "    --- " << std::endl;
    txt << "    symmetric:          " << isSymmetric() << std::endl;
    txt << "    transitive:         " << isTransitive() << std::endl;
    txt << "    functional:         " << isFunctional() << std::endl;
    txt << "    inverse functional: " << isInverseFunctional() << std::endl;

    return txt.str();
}

} // end namespace owl_om
