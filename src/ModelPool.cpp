#include "ModelPool.hpp"
#include <sstream>
#include <numeric/LimitedCombination.hpp>
#include <organization_model/OrganizationModel.hpp>

namespace organization_model {

ModelPool::ModelPool()
    : std::map<owlapi::model::IRI, size_t>()
{}

ModelPool::ModelPool(const ModelCombination& modelCombination)
    : std::map<owlapi::model::IRI, size_t>()
{
    ModelCombination::const_iterator cit = modelCombination.begin();
    for(; cit != modelCombination.end(); ++cit)
    {
        const owlapi::model::IRI& model = *cit;
        (*this)[model] += 1;
    }
}

void ModelPool::setResourceCount(const owlapi::model::IRI& resource, size_t count)
{
    (*this)[resource] = count;
}

std::string ModelPool::toString() const
{
    std::stringstream ss;
    ModelPool::const_iterator cit = this->begin();
    ss << "ModelPool:" << std::endl;
    for(; cit != this->end(); ++cit)
    {
        ss << "    " << cit->first << " : " << cit->second << std::endl;
    }
    return ss.str();
}


ModelPool ModelPool::applyUpperBound(const ModelPool& upperBounds) const
{
    ModelPool modelPool;

    ModelPool::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        const owlapi::model::IRI& model = cit->first;
        ModelPool::const_iterator bit = upperBounds.find(model);
        if(bit != upperBounds.end())
        {
            modelPool[model] = std::min(cit->second, bit->second);
        }
    }
    return modelPool;
}

ModelCombination ModelPool::toModelCombination() const
{
    ModelCombination combination;
    ModelPool::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        for(size_t i = 0; i < cit->second; ++i)
        {
            combination.push_back(cit->first);
        }
    }
    return combination;
}

ModelPoolSet ModelPool::allCombinations() const
{
    ModelPoolSet allCombinations;

    ModelCombination modelCombinationBound = toModelCombination();
    numeric::LimitedCombination<owlapi::model::IRI> combinations(*this,
            numeric::LimitedCombination<owlapi::model::IRI>::totalNumberOfAtoms(*this), numeric::MAX);

    do {
        owlapi::model::IRIList combination = combinations.current();
        ModelPool modelPool = OrganizationModel::combination2ModelPool(combination);
        allCombinations.insert(modelPool);
    } while(combinations.next());

    return allCombinations;
}

ModelPoolDelta::ModelPoolDelta(const ModelPool& pool)
{
    ModelPool::const_iterator cit = pool.begin();
    for(; cit != pool.end(); ++cit)
    {
        insert(std::pair<owlapi::model::IRI, int>(cit->first, cit->second));
    }
}

bool ModelPoolDelta::isNegative() const
{
    ModelPoolDelta::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(cit->second < 0)
        {
            return true;
        }
    }
    return false;
}

ModelPool ModelPoolDelta::toModelPool() const
{
    ModelPool modelPool;
    ModelPoolDelta::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(cit->second < 0)
        {
            throw std::runtime_error("organization_model::ModelPoolDelta contains negative values -- cannot convert to ModelPool");
        } else if(cit->second > 0)
        {
            modelPool[cit->first] = cit->second;
        }
    }
    return modelPool;
}

std::string ModelPoolDelta::toString() const
{
    std::stringstream ss;
    ModelPoolDelta::const_iterator cit = this->begin();
    ss << "ModelPoolDelta:" << std::endl;
    for(; cit != this->end(); ++cit)
    {
        ss << "    " << cit->first << " : " << cit->second << std::endl;
    }
    return ss.str();
}

std::vector<owlapi::model::IRI> ModelPoolDelta::getModels(const ModelPool& modelPool)
{
    std::vector<owlapi::model::IRI> models;
    ModelPool::const_iterator cit  = modelPool.begin();
    for(; cit != modelPool.end(); ++cit)
    {
        models.push_back(cit->first);
    }
    return models;
}

std::string ModelPool::toString(const ModelPoolSet& modelPoolSet, uint32_t indent)
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace;

    ModelPoolSet::const_iterator cit = modelPoolSet.begin();
    for(; cit != modelPoolSet.end();++cit)
    {
        ModelCombination combination = cit->toModelCombination();
        ss << owlapi::model::IRI::toString(combination, true);
        if(cit != modelPoolSet.end())
        {
            ss << ",";
        }
    }
    return ss.str();
}


} // end namespace organization_model
