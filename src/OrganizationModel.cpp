#include "OrganizationModel.hpp"

#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/model/OWLOntologyReader.hpp>

using namespace owlapi::model;

namespace organization_model {

bool Service::operator<(const Service& other) const
{
    return this->getModel() < other.getModel();
}

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
        ss << "    combination:    " << IRI::toString(cit->first, true) << std::endl;
        ss << "      --> functions: " << IRI::toString(cit->second, true) << std::endl;
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
        ss << "    function:    " << cit->first.getFragment() << std::endl;
        ss << "        supported by:" << std::endl;

        const ModelCombinationList& combinationsList = cit->second;
        ModelCombinationList::const_iterator comIt = combinationsList.begin();
        for(; comIt != combinationsList.end(); ++comIt)
        {
            ss << "        combination:    " << IRI::toString(*comIt, true) << std::endl;
        }
    }

    return ss.str();
}

ModelPool OrganizationModel::combination2ModelPool(const ModelCombination& combination)
{
    ModelPool modelPool;
    ModelCombination::const_iterator cit = combination.begin();
    for(; cit != combination.end(); ++cit)
    {
        modelPool[*cit] += 1;
    }
    return modelPool;
}

ModelCombination OrganizationModel::modelPool2Combination(const ModelPool& pool)
{
    ModelCombination combination;
    ModelPool::const_iterator cit = pool.begin();
    for(; cit != pool.end(); ++cit)
    {
        owlapi::model::IRI model = cit->first;
        size_t modelCount = cit->second;

        for(size_t i = 0; i < modelCount; ++i)
        {
            combination.push_back(model);
        }
    }
    std::sort(combination.begin(), combination.end());
    return combination;
}

} // end namespace organization_model
