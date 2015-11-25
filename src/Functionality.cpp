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

FunctionalitySet Functionality::toFunctionalitySet(const owlapi::model::IRIList& models)
{
    FunctionalitySet functionalities;
    {
        IRIList::const_iterator cit = models.begin();
        for(; cit != models.end(); ++cit)
        {
            functionalities.insert( Functionality(*cit) );
        }
    }
    return functionalities;
}

} // end namespace organization_model
