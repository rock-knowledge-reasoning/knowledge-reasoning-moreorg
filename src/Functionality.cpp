#include "Functionality.hpp"

using namespace owlapi::model;

namespace organization_model {

Functionality::Functionality(const owlapi::model::IRI& model)
    : mModel(model)
{}

bool Functionality::operator<(const Functionality& other) const
{
    return this->getModel() < other.getModel();
}

Functionality::Set Functionality::toFunctionalitySet(const owlapi::model::IRIList& models)
{
    Functionality::Set functionalities;
    {
        IRIList::const_iterator cit = models.begin();
        for(; cit != models.end(); ++cit)
        {
            functionalities.insert( Functionality(*cit) );
        }
    }
    return functionalities;
}

std::string Functionality::toString(const std::set<Functionality>& functionalities)
{
    std::string s;
    s += "[";
    Functionality::Set::const_iterator cit = functionalities.begin();
    for(; cit != functionalities.end();)
    {
        s += cit->toString();
        ++cit;

        if( cit != functionalities.end())
        {
            s += ", ";
        }
    }
    s += "]";
    return s;
}

} // end namespace organization_model
