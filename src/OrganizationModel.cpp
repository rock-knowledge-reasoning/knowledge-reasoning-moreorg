#include "OrganizationModel.hpp"

#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/model/OWLOntologyReader.hpp>

using namespace owlapi::model;

namespace organization_model {

OrganizationModel::OrganizationModel(const std::string& filename)
    : mpOntology( new OWLOntology())
{
    if(!filename.empty())
    {
        OWLOntologyReader reader;
        mpOntology = reader.fromFile(filename);
    }

    mpTell = OWLOntologyTell::Ptr( new OWLOntologyTell(mpOntology));
    mpAsk = OWLOntologyAsk::Ptr( new OWLOntologyAsk(mpOntology));
}

OrganizationModel OrganizationModel::copy() const
{
    OrganizationModel om;
    om.mpOntology = OWLOntology::Ptr( new OWLOntology( *ontology().get() ));
    return om;
}

std::string OrganizationModel::toString(const Combination2FunctionMap& combinationFunctionMap)
{
    std::stringstream ss;
    ss << "combination --> functions: " << std::endl;
    Combination2FunctionMap::const_iterator cit = combinationFunctionMap.begin();
    for(; cit != combinationFunctionMap.end(); ++cit)
    {
        ss << "    combination:    " << cit->first << std::endl;
        ss << "      --> functions: " << cit->second << std::endl;
    }
    return ss.str();
}

std::string OrganizationModel::toString(const Function2CombinationMap& functionCombinationMap)
{
    std::stringstream ss;
    ss << "function --> combinations: ";
    Function2CombinationMap::const_iterator cit = functionCombinationMap.begin();
    for(; cit != functionCombinationMap.end(); ++cit)
    {
        ss << "    function:    " << cit->first << std::endl;
        ss << "        supported by:" << std::endl;

        const ModelCombinationList& combinationsList = cit->second;
        ModelCombinationList::const_iterator comIt = combinationsList.begin();
        for(; comIt != combinationsList.end(); ++comIt)
        {
            ss << "        combination:    " << *comIt << std::endl;
        }
    }

    return ss.str();
}


} // end namespace organization_model
