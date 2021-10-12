#include "ModelPool.hpp"
#include <sstream>
#include <numeric/LimitedCombination.hpp>
#include "OrganizationModel.hpp"
#include "Algebra.hpp"

namespace moreorg {

ModelPool::ModelPool()
{}

ModelPool::ModelPool(const ModelCombination& modelCombination)
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

std::string ModelPool::toString(uint32_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ModelPool::const_iterator cit = this->begin();
    ss << hspace << "ModelPool:" << std::endl;
    for(; cit != this->end(); ++cit)
    {
        ss << "    " << hspace << cit->first << " : " << cit->second << std::endl;
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


ModelCombinationSet ModelPool::applyUpperBound(const ModelCombinationSet& combinations, const ModelPool& upperBound)
{
    ModelCombinationSet boundedCombinations;
    ModelCombinationSet::const_iterator cit = combinations.begin();
    for(; cit != combinations.end(); ++cit)
    {
        ModelPool modelPool = OrganizationModel::combination2ModelPool(*cit);
        if(modelPool.isWithinUpperBound(upperBound))
        {
            boundedCombinations.insert(*cit);
        }
    }

    LOG_DEBUG_S << "Upper bound set on resources: " << std::endl
        << "prev: " << OrganizationModel::toString(combinations) << std::endl
        << "bound: " << std::endl << ModelPoolDelta(upperBound).toString(4) << std::endl
        << "bounded: " << OrganizationModel::toString(boundedCombinations);
    return boundedCombinations;
}

ModelPool::Set ModelPool::applyLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBound)
{
    ModelPool::Set boundedModelPools;
    ModelPool::Set::const_iterator cit = modelPools.begin();
    for(; cit != modelPools.end(); ++cit)
    {
        const ModelPool& modelPool = *cit;
        ModelPoolDelta delta = Algebra::substract(lowerBound, modelPool);
        if(!delta.isNegative())
        {
            boundedModelPools.insert(modelPool);
        }
    }

    LOG_DEBUG_S << "Lower bound set on resources: " << std::endl
        << "prev: " << std::endl << ModelPool::toString(modelPools,4) << std::endl
        << "bound: " << std::endl << ModelPoolDelta(lowerBound).toString(4) << std::endl
        << "bounded: " << std::endl << ModelPool::toString(boundedModelPools,4);
    return boundedModelPools;
}

ModelCombinationSet ModelPool::applyLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBound)
{
    ModelCombinationSet boundedCombinations;
    ModelCombinationSet::const_iterator cit = combinations.begin();
    for(; cit != combinations.end(); ++cit)
    {
        ModelPool modelPool = OrganizationModel::combination2ModelPool(*cit);
        ModelPoolDelta delta = Algebra::substract(lowerBound, modelPool);
        if(!delta.isNegative())
        {
            boundedCombinations.insert(*cit);
        }
    }

    LOG_DEBUG_S << "Lower bound set on resources: " << std::endl
        << "prev: " << OrganizationModel::toString(combinations) << std::endl
        << "bound: " << std::endl << ModelPoolDelta(lowerBound).toString() << std::endl
        << "bounded: " << OrganizationModel::toString(boundedCombinations);
    return boundedCombinations;
}

ModelPool::Set ModelPool::expandToLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBound)
{
    ModelPool::Set boundedModelPools;
    if(modelPools.empty())
    {
        boundedModelPools.insert(lowerBound);
        return boundedModelPools;
    }

    ModelPool::Set::const_iterator cit = modelPools.begin();
    for(; cit != modelPools.end(); ++cit)
    {
        ModelPool modelPool = *cit;
        // enforce minimum requirement
        modelPool = Algebra::max(lowerBound, modelPool);
        boundedModelPools.insert(modelPool);
    }

    LOG_DEBUG_S << "Lower bound expanded on resources: " << std::endl
        << "prev: " << std::endl << ModelPool::toString(modelPools,4) << std::endl
        << "bound: " << std::endl << ModelPoolDelta(lowerBound).toString(4) << std::endl
        << "bounded: " << std::endl << ModelPool::toString(boundedModelPools,4);
    return boundedModelPools;
}

ModelCombinationSet ModelPool::expandToLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBound)
{
    ModelCombinationSet boundedCombinations;
    if(combinations.empty())
    {
        boundedCombinations.insert(OrganizationModel::modelPool2Combination(lowerBound));
    }
    ModelCombinationSet::const_iterator cit = combinations.begin();
    for(; cit != combinations.end(); ++cit)
    {
        ModelPool modelPool = OrganizationModel::combination2ModelPool(*cit);
        // enforce minimum requirement
        modelPool = Algebra::max(lowerBound, modelPool);
        ModelCombination expandedCombination = OrganizationModel::modelPool2Combination(modelPool);

        boundedCombinations.insert(expandedCombination);
    }

    LOG_DEBUG_S << "Lower bound expanded on resources: " << std::endl
        << "prev: " << OrganizationModel::toString(combinations) << std::endl
        << "bound: " << std::endl << ModelPoolDelta(lowerBound).toString(4) << std::endl
        << "bounded: " << OrganizationModel::toString(boundedCombinations);
    return boundedCombinations;
}


bool ModelPool::isWithinUpperBound(const ModelPool& upperBound) const
{
    for(const ModelPool::value_type& v : upperBound)
    {
        ModelPool::const_iterator cit = this->find(v.first);
        if(cit != this->end())
        {
            if(cit->second > v.second)
            {
                return false;
            }
        }
    }
    return true;
}

ModelPool::Set ModelPool::applyUpperBound(const ModelPool::Set& modelPools, const ModelPool& upperBound)
{
    ModelPool::Set boundedModelPools;
    ModelPool::Set::const_iterator cit = modelPools.begin();
    for(; cit != modelPools.end(); ++cit)
    {
        const ModelPool& modelPool = *cit;
        if(modelPool.isWithinUpperBound(upperBound))
        {
            boundedModelPools.insert(modelPool);
        }
    }

    LOG_DEBUG_S << "Upper bound set on resources: " << std::endl
        << "    prev: " << ModelPool::toString(modelPools,4) << std::endl
        << "    bound: " << ModelPoolDelta(upperBound).toString(4) << std::endl
        << "    bounded: " << ModelPool::toString(boundedModelPools,4);
    return boundedModelPools;
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

ModelPool::Set ModelPool::allCombinations(size_t maxSize) const
{
    ModelPool::Set allCombinations;

    ModelCombination modelCombinationBound = toModelCombination();

    // Account for the maximum size of the combination
    size_t sizeOfCombination = numeric::LimitedCombination<owlapi::model::IRI>::totalNumberOfAtoms(*this);
    if(maxSize > 0)
    {
        sizeOfCombination = std::min(maxSize, sizeOfCombination);
    }

    numeric::LimitedCombination<owlapi::model::IRI> combinations(*this,
            sizeOfCombination,
            numeric::MAX);

    do {
        owlapi::model::IRIList combination = combinations.current();
        ModelPool modelPool = OrganizationModel::combination2ModelPool(combination);
        allCombinations.insert(modelPool);
    } while(combinations.next());

    return allCombinations;
}

const owlapi::model::IRI& ModelPool::getAtomic() const
{
    if(isAtomic())
    {
        return begin()->first;
    } else {
        throw std::runtime_error("moreorg::ModelPool::getAtomic: this model pool does"
                " not describe an atomic model");
    }
}

ModelPool ModelPool::compact() const
{
    ModelPool pool;
    for(const ModelPool::value_type& v : *this)
    {
        if(v.second != 0)
        {
            pool.insert(v);
        }
    }
    return pool;
}

bool ModelPool::isNull() const
{
    ModelPool::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(cit->second != 0)
        {
            return false;
        }
    }
    return true;
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

bool ModelPoolDelta::isNull() const
{
    ModelPoolDelta::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(cit->second != 0)
        {
            return false;
        }
    }
    return true;
}

ModelPool ModelPoolDelta::toModelPool() const
{
    ModelPool modelPool;
    ModelPoolDelta::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(cit->second < 0)
        {
            throw std::runtime_error("moreorg::ModelPoolDelta contains negative values -- cannot convert to ModelPool");
        } else if(cit->second > 0)
        {
            modelPool[cit->first] = cit->second;
        }
    }
    return modelPool;
}

std::string ModelPoolDelta::toString(uint32_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent,' ');
    ModelPoolDelta::const_iterator cit = this->begin();
    ss << hspace << "ModelPoolDelta:" << std::endl;
    for(; cit != this->end(); ++cit)
    {
        ss << "    " << hspace << cit->first << " : " << cit->second << std::endl;
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

std::string ModelPool::toString(const ModelPool::Set& modelPoolSet, uint32_t indent)
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace;

    ModelPool::Set::const_iterator cit = modelPoolSet.begin();
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

std::string ModelPool::toString(const ModelPool::List& modelPoolList, uint32_t indent)
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace;

    ModelPool::List::const_iterator cit = modelPoolList.begin();
    for(; cit != modelPoolList.end();++cit)
    {
        ModelCombination combination = cit->toModelCombination();
        ss << owlapi::model::IRI::toString(combination, true);
        if(cit != modelPoolList.end())
        {
            ss << ",";
        }
    }
    return ss.str();
}

size_t ModelPool::getValue(const owlapi::model::IRI& resource, size_t defaultVal) const
{
    std::map<owlapi::model::IRI, size_t>::const_iterator cit =  this->find(resource);
    if(cit != this->end())
    {
        return cit->second;
    }
    return defaultVal;
}

std::pair<owlapi::model::IRI, size_t> ModelPool::getMaxResource() const
{
    size_t maxValue = std::numeric_limits<size_t>::min();
    //moreorg::ModelPool::value_type currentMax;
    std::pair<owlapi::model::IRI, size_t> currentMax;

    moreorg::ModelPool::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(maxValue < cit->second)
        {
            maxValue = cit->second;
            currentMax = *cit;
        }
        maxValue = std::max( maxValue, cit->second);
    }
    return currentMax;
}

std::pair<owlapi::model::IRI, size_t> ModelPool::getMinResource() const
{
    size_t minValue = std::numeric_limits<size_t>::max();
    std::pair<owlapi::model::IRI, size_t> currentMin;

    moreorg::ModelPool::const_iterator cit = this->begin();
    for(; cit != this->end(); ++cit)
    {
        if(cit->second < minValue )
        {
            minValue = cit->second;
            currentMin = *cit;
        }
    }
    return currentMin;
}

size_t ModelPool::getMaxResourceCount() const
{
    return getMaxResource().second;
}

size_t ModelPool::getMinResourceCount() const
{
    return getMinResource().second;
}

owlapi::model::IRIList ModelPool::getModels() const
{
    owlapi::model::IRIList models;
    for(const ModelPool::value_type& m : *this)
    {
        models.push_back(m.first);
    }
    return models;
}

} // end namespace moreorg
