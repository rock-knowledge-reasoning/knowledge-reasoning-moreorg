#include "OrganizationModel.hpp"
#include "OrganizationModelAsk.hpp"
#include "OrganizationModelTell.hpp"
#include <owlapi/io/OWLOntologyIO.hpp>

using namespace owlapi::model;

namespace organization_model {

OrganizationModel::OrganizationModel(const std::string& filename)
    : mpOntology( new OWLOntology())
{
    if(!filename.empty())
    {
        mpOntology = owlapi::io::OWLOntologyIO::fromFile(filename);
    }
}

OrganizationModel OrganizationModel::copy() const
{
    OrganizationModel om;
    om.mpOntology = make_shared<OWLOntology>(*ontology().get());
    return om;
}

std::string OrganizationModel::toString(const Pool2FunctionMap& combinationFunctionMap)
{
    std::stringstream ss;
    ss << "pool --> functions: " << std::endl;
    Pool2FunctionMap::const_iterator cit = combinationFunctionMap.begin();
    for(; cit != combinationFunctionMap.end(); ++cit)
    {
        ss << "    pool:    " << IRI::toString((cit->first).toModelCombination(), true) << std::endl;
        ss << "      --> functions: " << IRI::toString(cit->second, true) << std::endl;
    }
    return ss.str();
}

std::string OrganizationModel::toString(const Function2PoolMap& functionCombinationMap)
{
    std::stringstream ss;
    ss << "function --> combinations: ";
    Function2PoolMap::const_iterator cit = functionCombinationMap.begin();
    for(; cit != functionCombinationMap.end(); ++cit)
    {
        ss << "    function:    " << cit->first.getFragment() << std::endl;
        ss << "        supported by:" << std::endl;
        ss << "        combination:    " << ModelPool::toString(cit->second) << std::endl;
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
    return make_shared<OrganizationModel>(filename);
}

} // end namespace organization_model
