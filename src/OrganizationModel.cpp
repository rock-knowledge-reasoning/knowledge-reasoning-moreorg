#include "OrganizationModel.hpp"
#include "OrganizationModelAsk.hpp"
#include "OrganizationModelTell.hpp"
#include <owlapi/model/OWLOntologyReader.hpp>
#include <boost/make_shared.hpp>

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
}

OrganizationModel OrganizationModel::copy() const
{
    OrganizationModel om;
    om.mpOntology = boost::make_shared<OWLOntology>(*ontology().get());
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

        const ModelCombinationSet& combinationsSet = cit->second;
        ModelCombinationSet::const_iterator comIt = combinationsSet.begin();
        for(; comIt != combinationsSet.end(); ++comIt)
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

std::string OrganizationModel::toString(const ModelCombinationSet& combinations)
{
    std::stringstream ss;
    ModelCombinationSet::const_iterator cit = combinations.begin();
    for(; cit != combinations.end();)
    {
        ss << owlapi::model::IRI::toString(*cit, true);
        ++cit;
        if(cit != combinations.end())
        {
            ss << ",";
        }
    }
    return ss.str();
}

OrganizationModel::Ptr OrganizationModel::getInstance(const std::string& filename)
{
    return boost::make_shared<OrganizationModel>(filename);
}

} // end namespace organization_model
