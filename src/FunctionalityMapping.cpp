#include "FunctionalityMapping.hpp"
#include <sstream>

namespace organization_model {

FunctionalityMapping::FunctionalityMapping()
{}

FunctionalityMapping::FunctionalityMapping(const ModelPool& modelPool,
        const owlapi::model::IRIList& services,
        const ModelPool& functionalSaturationBound)
    : mModelPool(modelPool)
    , mServices(services)
    , mFunctionalSaturationBound(functionalSaturationBound)
{
    owlapi::model::IRIList::const_iterator cit = mServices.begin();
    for(; cit != mServices.end(); ++cit)
    {
        // initialize the set of functionalities
        mFunction2Combination[*cit] = ModelCombinationSet();
    }
}

const owlapi::model::IRIList& FunctionalityMapping::getFunction(const ModelCombination& combination) const
{
    // TODO: consider using ModelPool as representation for ModelCombination
    ModelPool modelPool(combination);
    ModelPool boundedPool = modelPool.applyUpperBound(mFunctionalSaturationBound);
    ModelCombination boundedCombination = boundedPool.toModelCombination();

    Combination2FunctionMap::const_iterator cit = mCombination2Function.find(boundedCombination);
    if(cit != mCombination2Function.end())
    {
        return cit->second;
    } else {
        throw std::invalid_argument("organization_model::FunctionalityMapping::getFunction: could not find"
                " model combination: " + owlapi::model::IRI::toString(boundedCombination, true));
    }
}

const ModelCombinationSet& FunctionalityMapping::getCombinations(const owlapi::model::IRI& iri) const
{
    Function2CombinationMap::const_iterator cit = mFunction2Combination.find(iri);
    if(cit != mFunction2Combination.end())
    {
        return cit->second;
    } else {
        throw std::invalid_argument("organization_model::FunctionalityMapping::getCombinations: could not find"
                " model combinations with function: " + iri.toString());
    }
}

void FunctionalityMapping::addFunction(const ModelCombination& combination,
        const owlapi::model::IRIList& functionModels)
{
    mCombination2Function[combination] = functionModels;
}

void FunctionalityMapping::add(const ModelCombination& combination, const owlapi::model::IRIList& functionModels)
{
    using namespace owlapi::model;
    addFunction(combination, functionModels);
    IRIList::const_iterator cit = functionModels.begin();
    for(; cit != functionModels.end(); ++cit)
    {
        IRI iri = *cit;
        mFunction2Combination[iri].insert(combination);
    }
}


std::string FunctionalityMapping::toString() const
{
    std::stringstream ss;
    ss << "FunctionalityMapping:" << std::endl;
    ss << mModelPool.toString() << std::endl;
    ss << "FunctionalSaturationBound -- ";
    ss << mFunctionalSaturationBound.toString() << std::endl;

    {
        Combination2FunctionMap::const_iterator cit = mCombination2Function.begin();
        ss << "Combination --> Functions:" << std::endl;
        for(; cit != mCombination2Function.end(); ++cit)
        {
            ss << "   - combination: " << owlapi::model::IRI::toString(cit->first, true) << std::endl;
            ss << "     function: " << owlapi::model::IRI::toString(cit->second, true) << std::endl;
        }
    }
    {
        Function2CombinationMap::const_iterator cit = mFunction2Combination.begin();
        ss << "Function --> Combination:" << std::endl;
        for(; cit != mFunction2Combination.end(); ++cit)
        {
            ss << "   - function: " << cit->first.toString() << std::endl;
            ss << "     combinations: " << std::endl;
            const ModelCombinationSet& combinations = cit->second;
            ModelCombinationSet::const_iterator sit = combinations.begin();
            for(; sit != combinations.end(); ++sit)
            {
                ss << "        " << owlapi::model::IRI::toString(*sit, true) << std::endl;
            }
        }
    }

    return ss.str();
}

} // end namespace organization_model
