#include "OrganizationModelAsk.hpp"
#include "Algebra.hpp"
#include <sstream>
#include <base-logging/Logging.hpp>
#include <base/Time.hpp>
#include <numeric/LimitedCombination.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/Vocabulary.hpp>
#include <organization_model/reasoning/ResourceMatch.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include <organization_model/algebra/Connectivity.hpp>


using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace organization_model {

OrganizationModelAsk::OrganizationModelAsk(const OrganizationModel::Ptr& om,
        const ModelPool& modelPool,
        bool applyFunctionalSaturationBound)
    : mpOrganizationModel(om)
    , mOntologyAsk(om->ontology())
    , mApplyFunctionalSaturationBound(applyFunctionalSaturationBound)
{
    if(!modelPool.empty())
    {
        if(!mApplyFunctionalSaturationBound)
        {
            LOG_INFO_S << "No functional saturation bound requested: this might take some time to prepare the functionality mappings";
        }
        prepare(modelPool, mApplyFunctionalSaturationBound);
    } else {
        LOG_INFO_S << "No model pool provided: did not prepare functionality mappings";
    }
}

void OrganizationModelAsk::prepare(const ModelPool& modelPool, bool applyFunctionalSaturationBound)
{
    mModelPool = modelPool;
    mFunctionalityMapping = computeFunctionalityMapping(mModelPool, applyFunctionalSaturationBound);
}

owlapi::model::IRIList OrganizationModelAsk::getServiceModels() const
{
    bool directSubclassOnly = false;
    IRIList subclasses = mOntologyAsk.allSubClassesOf(vocabulary::OM::Service(), directSubclassOnly);
    return subclasses;
}

owlapi::model::IRIList OrganizationModelAsk::getFunctionalities() const
{
    bool directSubclassOnly = false;
    IRIList subclasses = mOntologyAsk.allSubClassesOf(vocabulary::OM::Functionality(), directSubclassOnly);
    LOG_DEBUG_S << "Functionalities: " << subclasses;

    return subclasses;
}

FunctionalityMapping OrganizationModelAsk::computeFunctionalityMapping(const ModelPool& modelPool, bool applyFunctionalSaturationBound) const
{
    if(modelPool.empty())
    {
        throw std::invalid_argument("organization_model::OrganizationModel::computeFunctionalityMaps"
                " cannot compute functionality map for empty model pool");
    }

    IRIList functionalityModels = getFunctionalities();
    if(functionalityModels.empty())
    {
        throw std::runtime_error("organization_model::OrganizationModelAsk::computeFunctionalityMapping: available functionalities empty");
    }

    if(applyFunctionalSaturationBound)
    {
        return computeBoundedFunctionalityMapping(modelPool, functionalityModels);
    } else {
        return computeUnboundedFunctionalityMapping(modelPool, functionalityModels);
    }
}

FunctionalityMapping OrganizationModelAsk::computeBoundedFunctionalityMapping(const ModelPool& modelPool, const IRIList& functionalityModels) const
{
    std::pair<Pool2FunctionMap, Function2PoolMap> functionalityMaps;

    // TODO: create an bound on the model pool based on the given service
    // models based on the FunctionalSaturation -- avoids computing unnecessary
    // combination as function mappings
    //
    // LocationImageProvider: CREX --> 1, Sherpa --> 1
    // Compute service set of all known functionalities
    FunctionalitySet functionalities = Functionality::toFunctionalitySet(functionalityModels);

    // Compute the bound for all services
    LOG_DEBUG_S << "Get functional saturation bound for '" << functionalityModels;
    ModelPool functionalSaturationBound = getFunctionalSaturationBound(functionalities);
    LOG_DEBUG_S << "Functional saturation bound for '" << functionalityModels << "' is "
        << functionalSaturationBound.toString();

    // Apply bound to the existing model pool
    functionalSaturationBound = modelPool.applyUpperBound(functionalSaturationBound);
    LOG_INFO_S << "Model pool after applying the functional saturation bound for '" << functionalityModels << "' is "
        << functionalSaturationBound.toString();

    if(functionalSaturationBound.empty())
    {
        std::string msg = "organization_model::OrganizationModelAsk::computeBoundedFunctionalityMapping: provided empty functionalSaturationBound";
        msg += modelPool.toString() + "\n";
        msg += owlapi::model::IRI::toString(functionalityModels) + "\n";
        msg += functionalSaturationBound.toString() + "\n";
        throw std::runtime_error(msg);
    }

    FunctionalityMapping functionalityMapping(modelPool, functionalityModels, functionalSaturationBound);
    FunctionalitySet::const_iterator fit = functionalities.begin();
    for(; fit != functionalities.end(); ++fit)
    {
        const Functionality& functionality = *fit;
        ModelPool bound = getFunctionalSaturationBound(functionality);
        ModelPool boundedModelPool = functionalSaturationBound.applyUpperBound(bound);
        if(boundedModelPool.empty())
        {
            continue;
        }

        uint32_t numberOfAtoms = numeric::LimitedCombination<owlapi::model::IRI>::totalNumberOfAtoms(boundedModelPool);
        if(numberOfAtoms == 0)
        {
            LOG_INFO_S << "No support for " << functionality.toString();
            continue;
        }

        numeric::LimitedCombination<owlapi::model::IRI> limitedCombination(boundedModelPool, numberOfAtoms, numeric::MAX);
        do {
            IRIList combination = limitedCombination.current();
            ModelPool combinationModelPool = OrganizationModel::combination2ModelPool(combination);

            LOG_DEBUG_S << "Limited combination: " << std::endl
                << combinationModelPool.toString(4);


            FunctionalitySet functionalities;
            functionalities.insert(functionality);
            if(isMinimal(combinationModelPool, functionalities))
            {
                LOG_DEBUG_S << "combination is minimal for " << functionality.getModel().toString() << std::endl
                    << combinationModelPool.toString(4);
                if(algebra::Connectivity::isFeasible(combinationModelPool, *this))
                {
                    LOG_DEBUG_S << "combination is feasible " << std::endl
                    << combinationModelPool.toString(4);
                    functionalityMapping.add(combinationModelPool, functionality.getModel());
                }
            } else {
                LOG_DEBUG_S << "combination is not minimal for " << functionality.getModel().toString() << std::endl
                    << combinationModelPool.toString(4);
            }
        } while(limitedCombination.next());
    } // end for functionalities

    return functionalityMapping;
}

FunctionalityMapping OrganizationModelAsk::computeUnboundedFunctionalityMapping(const ModelPool& modelPool, const IRIList& functionalityModels) const
{
    std::pair<Pool2FunctionMap, Function2PoolMap> functionalityMaps;
    ModelPool functionalSaturationBound = modelPool;

    FunctionalityMapping functionalityMapping(modelPool, functionalityModels, functionalSaturationBound);
    const ModelPool& boundedModelPool = functionalityMapping.getFunctionalSaturationBound();

    // Compute now all feasible combinations (which have been bounded by the
    // functionality saturation bound)
    numeric::LimitedCombination<owlapi::model::IRI> limitedCombination(boundedModelPool,
            numeric::LimitedCombination<owlapi::model::IRI>::totalNumberOfAtoms(boundedModelPool), numeric::MAX);

    uint32_t count = 0;
    do {
        // Get the current model combination
        IRIList combination = limitedCombination.current();
        // Make sure we have a consistent ordering
        LOG_INFO_S << "Sort";
        {
            base::Time startTime = base::Time::now();
            std::sort(combination.begin(), combination.end());
            base::Time stopTime = base::Time::now();
            LOG_INFO_S << "   | --> required time: " << (stopTime - startTime).toSeconds();
        }

        LOG_DEBUG_S << "Check combination #" << ++count;
        LOG_DEBUG_S << "   | --> combination:             " << combination;
        LOG_DEBUG_S << "   | --> possible functionality models: " << functionalityModels;

        ModelPool combinationModelPool = OrganizationModel::combination2ModelPool(combination);

        // system that already provides full support for this functionality
        owlapi::model::IRIList::const_iterator cit = functionalityModels.begin();
        for(; cit != functionalityModels.end(); ++cit)
        {
            const Functionality& functionality = *cit;
            algebra::SupportType supportType = getSupportType(*cit, combinationModelPool);
            if(algebra::FULL_SUPPORT == supportType)
            {
                functionalityMapping.add(combinationModelPool, functionality.getModel());
            }
        }
    } while(limitedCombination.next());

    return functionalityMapping;
}


bool OrganizationModelAsk::isMinimal(const ModelPool& modelPool, const FunctionalitySet& functionalities) const
{
    // Check overall support
    algebra::SupportType supportType = getSupportType(functionalities, modelPool);
    if(algebra::FULL_SUPPORT != supportType)
    {
        LOG_INFO_S << "No full support for " << Functionality::toString(functionalities);
        return false;
    }

    LOG_INFO_S << "CheckMinimal: " << std::endl << modelPool.toString(4) << std::endl <<
        "   " << functionalities.begin()->getModel().toString();
    bool hasSingleModelFullSupport = false;
    bool hasSingleModelPartialSupport = false;
    // gather all models that provide only partial support
    ModelPool partialSupport;
    ModelPool::const_iterator mit = modelPool.begin();
    for(; mit != modelPool.end(); ++mit)
    {
        algebra::SupportType type = getSupportType(functionalities, mit->first, mit->second);
        LOG_DEBUG_S << "Support from: #" << mit->second << " of type " << mit->first << " is: "
            << algebra::SupportTypeTxt[type];
        switch(type)
        {
            case algebra::FULL_SUPPORT:
                hasSingleModelFullSupport = true;
                break;
            case algebra::PARTIAL_SUPPORT:
                hasSingleModelPartialSupport = true;
                partialSupport.insert(*mit);
                break;
            case algebra::NO_SUPPORT:
                break;
        }
    }
    if(hasSingleModelFullSupport)
    {
        LOG_INFO_S << "Full support: " << std::endl
            << modelPool.toString(4) << std::endl
            << "    for " << std::endl
            << "    " << Functionality::toString(functionalities);

        if(modelPool.size() == 1)
        {
            LOG_DEBUG_S << "    -- is minimal";
            // that is ok, since that is only a single systems
            return true;
        } else {
            LOG_DEBUG_S << "    -- is not minimal: one of the models is already providing full support";
            // this must be a redundant combination since one model is already
            // providing full support
            return false;
        }
    } else if(hasSingleModelPartialSupport)
    {
        LOG_DEBUG_S << "Partial support: " << std::endl
            << modelPool.toString(4) << std::endl
            << "    for " << std::endl
            << "    " << Functionality::toString(functionalities);

        // has partial support, thus check that for that particular
        // combination that it contains no redundancies, i.e. we cannot
        // remove any model instance to provide full support
        ModelPool::const_iterator pit = partialSupport.begin();
        for(; pit != partialSupport.end(); ++pit)
        {
            ModelPool reduced = partialSupport;
            --reduced[pit->first];

            algebra::SupportType reducedSupport = getSupportType(functionalities, reduced);
            // Is reduced, but still has full support, i.e. must be exceeding
            // saturation
            if(reducedSupport == algebra::FULL_SUPPORT)
            {
                // redundant combination
                LOG_DEBUG_S << "    -- is not minimal: it contains redundancies";
                return false;
            }
        }
    }

    LOG_DEBUG_S << "    -- is minimal: no redundancies identified";
    return true;
}

ModelPoolSet OrganizationModelAsk::filterNonMinimal(const ModelPoolSet& modelPoolSet, const FunctionalitySet& functionalities) const
{
    ModelPoolSet filtered;
    ModelPoolSet::const_iterator mit = modelPoolSet.begin();
    for(; mit != modelPoolSet.end(); ++mit)
    {
        if(isMinimal(*mit, functionalities))
        {
            filtered.insert(*mit);
        }
    }
    return filtered;
}

ModelPoolSet OrganizationModelAsk::getResourceSupport(const FunctionalitySet& functionalities) const
{
    if(functionalities.empty())
    {
        std::invalid_argument("organization_model::OrganizationModelAsk::getResourceSupport:"
                " no functionality given in request");
    }

    /// Retrieve the systems that support the functionalities and create
    //'compositions'
    /// i.e. per requested function the combination of models that support it,
    Function2PoolMap functionalityProviders;
    ModelPoolSet supportingCompositions;
    {
        FunctionalitySet::const_iterator cit = functionalities.begin();
        for(; cit != functionalities.end(); ++cit)
        {
            const Functionality& functionality = *cit;
            const owlapi::model::IRI& functionalityModel = functionality.getModel();
            try {
                 ModelPoolSet modelPoolSet = mFunctionalityMapping.getModelPools(functionalityModel);
                 supportingCompositions = Algebra::maxCompositions(supportingCompositions, modelPoolSet);
            } catch(const std::invalid_argument& e)
            {
                LOG_DEBUG_S << "Could not find resource support for service: '" << functionalityModel;
                return ModelPoolSet();
            }
        }
    }
    return supportingCompositions;
}

ModelPoolSet OrganizationModelAsk::getBoundedResourceSupport(const FunctionalitySet& functionalities) const
{
    ModelPoolSet modelPools = getResourceSupport(functionalities);
    modelPools = filterNonMinimal(modelPools, functionalities);
    ModelPool bound = getFunctionalSaturationBound(functionalities);
    return applyUpperBound(modelPools, bound);
}

ModelPoolSet OrganizationModelAsk::applyUpperBound(const ModelPoolSet& modelPools, const ModelPool& upperBound) const
{
    ModelPoolSet boundedModelPools;
    ModelPoolSet::const_iterator cit = modelPools.begin();
    for(; cit != modelPools.end(); ++cit)
    {
        const ModelPool& modelPool = *cit;

        LOG_DEBUG_S << "DELTA lval: " << std::endl
            << ModelPoolDelta(upperBound).toString(4);
        LOG_DEBUG_S << "DELTA rval: " << std::endl
            << ModelPoolDelta(modelPool).toString(4);
        ModelPoolDelta delta = Algebra::substract(modelPool, upperBound);
        LOG_DEBUG_S << "Result: " << std::endl
            << delta.toString(4);

        if(!delta.isNegative())
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

ModelCombinationSet OrganizationModelAsk::applyUpperBound(const ModelCombinationSet& combinations, const ModelPool& upperBound) const
{
    ModelCombinationSet boundedCombinations;
    ModelCombinationSet::const_iterator cit = combinations.begin();
    for(; cit != combinations.end(); ++cit)
    {
        ModelPool modelPool = OrganizationModel::combination2ModelPool(*cit);

        LOG_DEBUG_S << "DELTA lva: " << ModelPoolDelta(upperBound).toString(4);
        LOG_DEBUG_S << "DELTA rval: " << ModelPoolDelta(modelPool).toString(4);
        ModelPoolDelta delta = Algebra::substract(modelPool, upperBound);
        LOG_DEBUG_S << "Result: " << delta.toString(4);
        if(!delta.isNegative())
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

ModelPoolSet OrganizationModelAsk::applyLowerBound(const ModelPoolSet& modelPools, const ModelPool& lowerBound) const
{
    ModelPoolSet boundedModelPools;
    ModelPoolSet::const_iterator cit = modelPools.begin();
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

ModelCombinationSet OrganizationModelAsk::applyLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBound) const
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

ModelPoolSet OrganizationModelAsk::expandToLowerBound(const ModelPoolSet& modelPools, const ModelPool& lowerBound) const
{
    ModelPoolSet boundedModelPools;
    if(modelPools.empty())
    {
        boundedModelPools.insert(lowerBound);
        return boundedModelPools;
    }

    ModelPoolSet::const_iterator cit = modelPools.begin();
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

ModelCombinationSet OrganizationModelAsk::expandToLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBound) const
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

algebra::SupportType OrganizationModelAsk::getSupportType(const FunctionalitySet& functionalities, const owlapi::model::IRI& model, uint32_t cardinalityOfModel) const
{
    ModelPool modelPool;
    modelPool[model] = cardinalityOfModel;
    return getSupportType(functionalities, modelPool);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const Functionality& functionality, const owlapi::model::IRI& model, uint32_t cardinalityOfModel) const
{
    FunctionalitySet functionalities;
    functionalities.insert(functionality);
    return getSupportType(functionalities, model, cardinalityOfModel);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const FunctionalitySet& functionalities, const ModelPool& modelPool) const
{
    IRIList functionalityModels;
    FunctionalitySet::const_iterator fit = functionalities.begin();
    for(; fit != functionalities.end(); ++fit)
    {
        functionalityModels.push_back(fit->getModel());
    }

    // Define what is required
    algebra::ResourceSupportVector functionalitySupportVector = getSupportVector(functionalityModels, IRIList() /*filter labels*/, false /*useMaxCardinality*/);
    const IRIList& labels = functionalitySupportVector.getLabels();

    base::VectorXd zeroSupport = base::VectorXd::Zero(labels.size());
    algebra::ResourceSupportVector modelPoolSupportVector(zeroSupport, labels);

    // Gather what is available
    ModelPool::const_iterator cit = modelPool.begin();
    for(; cit != modelPool.end(); ++cit)
    {
        const IRI& model = cit->first;
        const uint32_t& cardinality = cit->second;

        algebra::ResourceSupportVector support = getSupportVector(model, labels, true)*static_cast<double>(cardinality);
        modelPoolSupportVector += support;
    }

    LOG_DEBUG_S << "Functionality support vector:" << functionalitySupportVector.toString(4);
    LOG_DEBUG_S << "Model support vector:" << modelPoolSupportVector.toString(4);

    return functionalitySupportVector.getSupportFrom(modelPoolSupportVector, *this);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const Functionality& functionality, const ModelPool& modelPool) const
{
    FunctionalitySet functionalities;
    functionalities.insert(functionality);
    return getSupportType(functionalities, modelPool);
}

uint32_t OrganizationModelAsk::getFunctionalSaturationBound(const owlapi::model::IRI& requirementModel, const owlapi::model::IRI& model) const
{
    LOG_DEBUG_S << "Get functional saturation bound for " << requirementModel << " for model '" << model << "'";
    // Collect requirements, i.e., max cardinalities
    algebra::ResourceSupportVector requirementSupportVector = getSupportVector(requirementModel, IRIList(), false /*useMaxCardinality*/);
    if(requirementSupportVector.isNull())
    {
        owlapi::model::IRIList labels;
        labels.push_back(requirementModel);
        base::VectorXd required(1);
        required(0) = 1;
        requirementSupportVector = algebra::ResourceSupportVector(required, labels);
        LOG_DEBUG_S << "functionality support vector is null : using " << requirementSupportVector.toString();
    } else {
        LOG_DEBUG_S << "Get model support vector";
    }
    // Collect available resources -- and limit to the required ones
    // (getSupportVector will accumulate all (subclass) models)
    algebra::ResourceSupportVector modelSupportVector = getSupportVector(model, requirementSupportVector.getLabels(), true /*useMaxCardinality*/);
    LOG_DEBUG_S << "Retrieved model support vector with labels: " << requirementSupportVector.getLabels();

    // Expand the support vectors to account for subclasses within the required
    // scope
    requirementSupportVector = requirementSupportVector.embedClassRelationship(*this);
    modelSupportVector = modelSupportVector.embedClassRelationship(*this);

    // Compute the support ratios
    algebra::ResourceSupportVector ratios = requirementSupportVector.getRatios(modelSupportVector);

    LOG_DEBUG_S << "Requirement: " << std::endl << requirementSupportVector.toString(4);
    LOG_DEBUG_S << "Provider: " << std::endl << modelSupportVector.toString(4);
    LOG_DEBUG_S << "Ratios: " << std::endl << ratios.toString(4);

    // max in the set of ratio tells us how many model instances
    // contribute to fulfill this service (even partially)
    double max = 0.0;
    for(uint32_t i = 0; i < ratios.size(); ++i)
    {
        double val = ratios(i);
        if(val != std::numeric_limits<double>::quiet_NaN())
        {
            if(val > max)
            {
                max = val;
            }
        }
    }
    return static_cast<uint32_t>( std::ceil(max) );
}

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const Functionality& functionality) const
{
    if(mModelPool.empty())
    {
        throw std::invalid_argument("organization_model::OrganizationModelAsk::getFunctionalSaturationBound:"
                " model pool is empty. Call OrganizationModelAsk::prepare with model pool");
    }

    ModelPool upperBounds;
    ModelPool::const_iterator cit = mModelPool.begin();
    for(; cit != mModelPool.end(); ++cit)
    {
        uint32_t saturation = getFunctionalSaturationBound(functionality.getModel(), cit->first);
        upperBounds[cit->first] = saturation;
    }
    return upperBounds;
}

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const FunctionalitySet& functionalities) const
{
    ModelPool upperBounds;
    FunctionalitySet::const_iterator cit = functionalities.begin();
    for(; cit != functionalities.end(); ++cit)
    {
        ModelPool saturation = getFunctionalSaturationBound(*cit);

        ModelPool::const_iterator mit = saturation.begin();
        for(; mit != saturation.end(); ++mit)
        {
            upperBounds[mit->first] = std::max(upperBounds[mit->first], saturation[mit->first]);
        }
    }
    return upperBounds;
}

bool OrganizationModelAsk::canBeDistinct(const ModelCombination& a, const ModelCombination& b) const
{
    ModelPool poolA = OrganizationModel::combination2ModelPool(a);
    ModelPool poolB = OrganizationModel::combination2ModelPool(b);

    ModelPoolDelta totalRequirements = Algebra::sum(poolA, poolB);
    ModelPoolDelta delta = Algebra::delta(totalRequirements, mModelPool);

    return delta.isNegative();
}

bool OrganizationModelAsk::isSupporting(const ModelPool& modelPool, const FunctionalitySet& functionalities) const
{
    // Requires the functionality mapping to be properly initialized

    FunctionalitySet::const_iterator cit = functionalities.begin();
    ModelPoolSet previousModelPools;
    bool init = true;
    for(; cit != functionalities.end(); ++cit)
    {
        const Functionality& functionality = *cit;
        try {
            const ModelPoolSet& modelPools = mFunctionalityMapping.getModelPools(functionality.getModel());

            if(init)
            {
                previousModelPools = modelPools;
                init = false;
                continue;
            }

            ModelPoolSet resultList;
            std::set_intersection(modelPools.begin(), modelPools.end(),
                    previousModelPools.begin(), previousModelPools.end(),
                    std::inserter(resultList, resultList.begin()) );
             previousModelPools.insert(resultList.begin(), resultList.end());

        } catch(const std::invalid_argument& e)
        {
            throw std::runtime_error("organization_model::OrganizationModelAsk::isSupporting \
                    could not find functionality '" + functionality.getModel().toString() + "'");
        }

    }

    ModelPoolSet::const_iterator pit = std::find(previousModelPools.begin(), previousModelPools.end(), modelPool);
    if(pit != previousModelPools.end())
    {
        return true;
    } else {
        return false;
    }
}

bool OrganizationModelAsk::isSupporting(const owlapi::model::IRI& model, const Functionality& functionality) const
{
    ModelPool modelPool;
    modelPool.setResourceCount(model, 1);

    FunctionalitySet functionalities;
    functionalities.insert(functionality);
    if( isSupporting(modelPool, functionalities) )
    {
        LOG_DEBUG_S << "model '" << model << "' supports '" << functionality.getModel() << "'";
        return true;
    } else {
        LOG_DEBUG_S << "model '" << model << "' does not support '" << functionality.getModel() << "'";
        return false;
    }
}


algebra::ResourceSupportVector OrganizationModelAsk::getSupportVector(const owlapi::model::IRIList& models,
        const owlapi::model::IRIList& filterLabels,
        bool useMaxCardinality) const
{
    using namespace owlapi::model;
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = mOntologyAsk.getCardinalityRestrictions(models, OWLCardinalityRestriction::MAX_OP);

    std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = restrictions.begin();
    if(restrictions.empty())
    {
        owlapi::model::IRIList labels;
        // Add only those models that are listed in the filterLabels list
        // since these are the requested once
        IRIList::const_iterator mit = models.begin();
        for(; mit != models.end(); ++mit)
        {
            const IRI& model = *mit;
            if(!filterLabels.empty())
            {
                IRIList::const_iterator lit = std::find(filterLabels.begin(), filterLabels.end(), model);
                if(lit == filterLabels.end())
                {
                    // not in list
                    continue;
                }
            }
            labels.insert(labels.begin(), models.begin(), models.end());
        }

        base::VectorXd available(labels.size());
        for(size_t i = 0; i < labels.size(); ++i)
        {
            available(i) = 1;
        }

        algebra::ResourceSupportVector supportVector(available, labels);
        return supportVector;

    } else {
        std::map<IRI, OWLCardinalityRestriction::MinMax> modelCount = OWLCardinalityRestriction::getBounds(restrictions);
        algebra::ResourceSupportVector supportVector = getSupportVector(modelCount, filterLabels, useMaxCardinality);
        LOG_DEBUG_S << "ModelCount: "<< modelCount.size() << ", restrictions: " << OWLCardinalityRestriction::toString(restrictions) << std::endl
            << supportVector.toString(4);
        return supportVector;
    }
}

algebra::ResourceSupportVector OrganizationModelAsk::getSupportVector(const owlapi::model::IRI& model,
        const owlapi::model::IRIList& filterLabels,
        bool useMaxCardinality) const
{
    IRIList models;
    models.push_back(model);

    return getSupportVector(models, filterLabels, useMaxCardinality);
}

algebra::ResourceSupportVector OrganizationModelAsk::getSupportVector(const std::map<owlapi::model::IRI,
    owlapi::model::OWLCardinalityRestriction::MinMax>& modelBounds,
    const owlapi::model::IRIList& filterLabels, bool useMaxCardinality) const
{
    if(modelBounds.size() == 0)
    {
        throw std::invalid_argument("organization_model::OrganizationModelAsk::getSupportVector: no model bounds given");
    }

    using namespace owlapi::model;

    base::VectorXd vector;
    std::vector<IRI> labels;

    if(filterLabels.empty())
    {
        // Create zero vector: each row maps to a model
        vector = base::VectorXd::Zero(modelBounds.size());

        std::map<IRI, OWLCardinalityRestriction::MinMax>::const_iterator cit = modelBounds.begin();
        uint32_t i = 0;
        for(; cit != modelBounds.end(); ++cit)
        {
            labels.push_back(cit->first);
            // using max value
            if(useMaxCardinality)
            {
                vector(i++) = cit->second.second;
            } else {
                vector(i++) = cit->second.first;
            }
        }
    } else {
        labels = filterLabels;
    }

    LOG_DEBUG_S << "Use labels: " << labels;
    vector = base::VectorXd::Zero(labels.size());

    std::vector<IRI>::const_iterator cit = labels.begin();
    uint32_t dimension = 0;
    for(; cit != labels.end(); ++cit)
    {
        const IRI& dimensionLabel = *cit;
        std::map<IRI, OWLCardinalityRestriction::MinMax>::const_iterator mit = modelBounds.begin();
        for(; mit != modelBounds.end(); ++mit)
        {
            const IRI& modelDimensionLabel = mit->first;
            LOG_DEBUG_S << "Check model support for " << dimensionLabel << "  from " << modelDimensionLabel;

            // Sum the requirement/availability of this model type
            if(dimensionLabel == modelDimensionLabel || mOntologyAsk.isSubClassOf(modelDimensionLabel, dimensionLabel))
            {
                if(useMaxCardinality)
                {
                    LOG_DEBUG_S << "update " << dimension << " with " << mit->second.second << " -- min is "<< mit->second.first;
                    // using max value
                    vector(dimension) += mit->second.second;
                } else {
                    // using min value
                    LOG_DEBUG_S << "update " << dimension << " with min " << mit->second.first << " -- max is "<< mit->second.second;
                    vector(dimension) += mit->second.first;
                }
            } else {
                LOG_DEBUG_S << "No support";
                // nothing to do
            }
        }
        ++dimension;
    }

    algebra::ResourceSupportVector supportVector(vector, labels);
    LOG_DEBUG_S << "Return support vector" << supportVector.toString(4);
    return supportVector;
}

std::string OrganizationModelAsk::toString() const
{
    std::stringstream ss;
    ss << "FunctionalityMapping:" << std::endl;
    ss << mFunctionalityMapping.toString() << std::endl;

    ModelPoolDelta mp(mModelPool);
    ss << mp.toString() << std::endl;
    return ss.str();
}

//owlapi::model::IRIList OrganizationModelAsk::filterSupportedModels(const owlapi::model::IRIList& combinations,
//        const owlapi::model::IRIList& serviceModels)
//{
//    using namespace owlapi::model;
//
//    std::vector<OWLCardinalityRestriction::Ptr> providerRestrictions =
//            ontology()->getCardinalityRestrictions(combinations);
//    owlapi::model::IRIList supportedModels;
//
//    owlapi::model::IRIList::const_iterator it = serviceModels.begin();
//    for(; it != serviceModels.end(); ++it)
//    {
//        owlapi::model::IRI serviceModel = *it;
//        std::vector<OWLCardinalityRestriction::Ptr> serviceRestrictions =
//            ontology()->getCardinalityRestrictions(serviceModel);
//
//        std::map<IRI, OWLCardinalityRestriction::MinMax> serviceModelCount = OWLCardinalityRestriction::getBounds(serviceRestrictions);
//        // get minimum requirements
//        bool useMaxCardinality = false;
//        ResourceSupportVector serviceSupportVector = getSupportVector(serviceModelCount, IRIList(), useMaxCardinality);
//
//        std::map<IRI, OWLCardinalityRestriction::MinMax> providerModelCount = OWLCardinalityRestriction::getBounds(providerRestrictions);
//        // Use all available
//        useMaxCardinality = true;
//        ResourceSupportVector providerSupportVector = getSupportVector(providerModelCount, serviceSupportVector.getLabels(), useMaxCardinality);
//
//        if( serviceSupportVector.fullSupportFrom(providerSupportVector) )
//        {
//            supportedModels.push_back(serviceModel);
//            LOG_DEBUG_S << "Full support for: ";
//            LOG_DEBUG_S << "    service model: " << serviceModel;
//            LOG_DEBUG_S << "    from combination of provider models: " << combinations;
//        }
//    }
//    return supportedModels;
//}

} // end namespace organization_model
