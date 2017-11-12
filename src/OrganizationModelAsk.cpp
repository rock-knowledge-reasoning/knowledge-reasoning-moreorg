#include "OrganizationModelAsk.hpp"
#include "Algebra.hpp"
#include <sstream>
#include <base-logging/Logging.hpp>
#include <base/Time.hpp>
#include <numeric/LimitedCombination.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/Vocabulary.hpp>
#include "reasoning/ResourceMatch.hpp"
#include "vocabularies/OM.hpp"
#include "algebra/Connectivity.hpp"
#include "FunctionalityRequirement.hpp"
#include "PropertyConstraintSolver.hpp"


using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace organization_model {

OrganizationModelAsk::OrganizationModelAsk(const OrganizationModel::Ptr& om,
        const ModelPool& modelPool,
        bool applyFunctionalSaturationBound,
        double feasibilityCheckTimeoutInMs
        )
    : mpOrganizationModel(om)
    , mOntologyAsk(om->ontology())
    , mApplyFunctionalSaturationBound(applyFunctionalSaturationBound)
    , mFeasibilityCheckTimeoutInMs(feasibilityCheckTimeoutInMs)
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
    for(const ModelPool::value_type& value : modelPool)
    {
        const IRI& model = value.first;
        if( mOntologyAsk.isSubClassOf(model, vocabulary::OM::Actor()))
        {
            mModelPool[model] = value.second;
        }
    }
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

ModelPool OrganizationModelAsk::getSupportedFunctionalities() const
{
    ModelPool modelPool;
    owlapi::model::IRISet mSupportedFunctionalities = mFunctionalityMapping.getSupportedFunctionalities();
    owlapi::model::IRISet::const_iterator cit = mSupportedFunctionalities.begin();
    for(; cit != mSupportedFunctionalities.end(); ++cit)
    {
        modelPool[*cit] = 1;
    }
    return modelPool;
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
    Functionality::Set functionalities = Functionality::toFunctionalitySet(functionalityModels);

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
    Functionality::Set::const_iterator fit = functionalities.begin();
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


            Functionality::Set functionalities;
            functionalities.insert(functionality);
            if(isMinimal(combinationModelPool, functionalities))
            {
                LOG_DEBUG_S << "combination is minimal for " << functionality.getModel().toString() << std::endl
                    << combinationModelPool.toString(4);
                if(algebra::Connectivity::isFeasible(combinationModelPool, *this, mFeasibilityCheckTimeoutInMs))
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


bool OrganizationModelAsk::isMinimal(const ModelPool& modelPool, const Functionality::Set& functionalities) const
{
    // Check overall support
    algebra::SupportType supportType = getSupportType(functionalities, modelPool);
    if(algebra::FULL_SUPPORT != supportType)
    {
        LOG_INFO_S << "No full support for " << Functionality::toString(functionalities) << " by " << std::endl << modelPool.toString(4);
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

double OrganizationModelAsk::getDataPropertyValue(const ModelPool& modelPool, const owlapi::model::IRI& dataProperty) const
{
    bool initialized = false;
    double value = 0;
    for(const ModelPool::value_type& p : modelPool)
    {
        try {
            const owlapi::model::IRI& model = p.first;
            if(!initialized)
            {
                value = mOntologyAsk.getDataValue(model, dataProperty)->getDouble();
                initialized = true;
            } else {
                value += mOntologyAsk.getDataValue(model, dataProperty)->getDouble();
            }
        } catch(const std::exception& e)
        {
            LOG_WARN_S << e.what();
        }
    }

    if(!initialized)
    {
        throw std::runtime_error("organization_model::OrganizationModelAsk::getDataPropertyValue: the data property '" + dataProperty.toString() + "' is not extractable for the model pool: " + modelPool.toString());
    }

    return value;
}

std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> OrganizationModelAsk::getCardinalityRestrictions(const ModelPool& modelPool, owlapi::model::OWLCardinalityRestriction::OperationType operationType, bool max2Min) const
{
    std::vector<OWLCardinalityRestriction::Ptr> allAvailableResources;

    owlapi::model::OWLProperty::Ptr propertyHas = ontology().getOWLObjectProperty( vocabulary::OM::has() );
    // Get model restrictions, i.e. in effect what has to be available for the
    // given models
    ModelPool::const_iterator mit = modelPool.begin();
    for(; mit != modelPool.end(); ++mit)
    {
        const IRI& model = mit->first;
        uint32_t modelCount = mit->second;

        std::vector<OWLCardinalityRestriction::Ptr> availableResources = mOntologyAsk.getCardinalityRestrictions(model, vocabulary::OM::has());

        // This is not a meta constraint, but a direct representation of an
        // atomic resource, so add the exact availability of the given high level resource, which is
        // defined through the model pool
        if(availableResources.empty())
        {
            OWLCardinalityRestriction::Ptr highlevelModelMaxConstraint = OWLCardinalityRestriction::getInstance(propertyHas,
                    modelCount,
                    mit->first,
                    OWLCardinalityRestriction::MAX);
            availableResources.push_back(highlevelModelMaxConstraint);

            OWLCardinalityRestriction::Ptr highlevelModelMinConstraint = OWLCardinalityRestriction::getInstance(propertyHas,
                    modelCount,
                    mit->first,
                    OWLCardinalityRestriction::MIN);
            availableResources.push_back(highlevelModelMinConstraint);
        }

        std::vector<OWLCardinalityRestriction::Ptr> available;
        for(OWLCardinalityRestriction::Ptr& restriction : availableResources)
        {
            // Update the cardinality with the actual model count
            uint32_t cardinality = modelCount*restriction->getCardinality();
            if(max2Min && restriction->getCardinalityRestrictionType() == OWLCardinalityRestriction::MAX)
            {
                OWLCardinalityRestriction::Ptr converted = OWLCardinalityRestriction::getInstance(restriction->getProperty(),
                        cardinality,
                        restriction->getQualification(),
                        OWLCardinalityRestriction::MIN);
                available.push_back(converted);
            } else {
                restriction->setCardinality(cardinality);
                available.push_back(restriction);
            }
        }

        allAvailableResources = owlapi::model::OWLCardinalityRestriction::join(allAvailableResources, available, operationType );
    }
    return allAvailableResources;
}

ModelPool::Set OrganizationModelAsk::filterNonMinimal(const ModelPool::Set& modelPoolSet, const Functionality::Set& functionalities) const
{
    ModelPool::Set filtered;
    ModelPool::Set::const_iterator mit = modelPoolSet.begin();
    for(; mit != modelPoolSet.end(); ++mit)
    {
        if(isMinimal(*mit, functionalities))
        {
            filtered.insert(*mit);
        }
    }
    return filtered;
}

ModelPool::Set OrganizationModelAsk::getResourceSupport(const FunctionalityRequirement& functionalityRequirement) const
{
    const owlapi::model::IRI& functionalityModel = functionalityRequirement.getFunctionality().getModel();

    // If there are no property constraints
    if(functionalityRequirement.getPropertyConstraints().empty())
    {
        return mFunctionalityMapping.getModelPools(functionalityModel);
    }

    try {
        // Computing all model combination that support this functionality
        ModelPool::Set modelPoolSet = mFunctionalityMapping.getModelPools(functionalityModel);
        ModelPool::Set supportPool;

        std::vector<double> scalingFactors = getScalingFactors(modelPoolSet, functionalityRequirement);

        size_t index = 0;
        for(const ModelPool& pool : modelPoolSet)
        {
            double scalingFactor = scalingFactors[index];
            if(scalingFactor == 0)
            {
                // nothing to do
            } else if(scalingFactor == 1.0)
            {
                supportPool.insert(pool);
            } else
            {
                ModelPool scaled = Algebra::multiply(pool, scalingFactor);
                supportPool.insert(scaled);
            }
            ++index;
        }

        ModelPool::Set boundedSupportPool;
        // Check if the scaled variant lies within general resource bounds
        if(mModelPool.empty())
        {
            LOG_WARN_S << "organization_model::OrganizationModelAsk::getResourceSupport: could not compute functionality constrained model pool, since model pool is missing for setting the upper bound";
            return supportPool;
        } else {
            return  ModelPool::applyUpperBound(supportPool, mModelPool);
        }
    } catch(const std::invalid_argument& e)
    {
        LOG_DEBUG_S << "Could not find resource support for service: '" << functionalityModel;
        return ModelPool::Set();
    }
}

ModelPool::Set OrganizationModelAsk::getResourceSupport(const FunctionalityRequirement::Map& functionalitiesRequirements) const
{
    ModelPool::Set supportingCompositions;
    for(const FunctionalityRequirement::Map::value_type& pair : functionalitiesRequirements)
    {
        // Requesting from the functionality mapping will in most cases
        // embed the functional saturation bound, e.g.
        // the minimum to provide a certain functionality (per se, without
        // accouting for a particular dataProperty requirement)
        //
        // In principle one will need a special join function for each kind
        // of data property -- right now we assume that we can 'sum' the
        // dataproperties in order to deal with the requirements
        //
        try {
            // deal with additional constraints
            const FunctionalityRequirement& functionalityRequirement = pair.second;
            ModelPool::Set modelPoolSet = getResourceSupport(functionalityRequirement);
            supportingCompositions = Algebra::maxCompositions(supportingCompositions, modelPoolSet);
        } catch(const std::invalid_argument& e)
        {
            LOG_DEBUG_S << "Could not find resource support for service: '" << pair.first.getModel();
            return ModelPool::Set();
        }
    }

    return supportingCompositions;
}

ModelPool::Set OrganizationModelAsk::getResourceSupport(const Functionality::Set& functionalities, const FunctionalityRequirement::Map& functionalityRequirements) const
{
    // Functionality requirements:
    if(functionalities.empty())
    {
        std::invalid_argument("organization_model::OrganizationModelAsk::getResourceSupport:"
                " no functionality given in request");
    }

    /// Retrieve the systems that support the functionalities and create
    /// 'compositions'
    /// i.e. per requested function the combination of models that support it,
    /// while accounting for the (data property constraints)
    Function2PoolMap functionalityProviders;
    ModelPool::Set supportingCompositions;
    {
        Functionality::Set::const_iterator cit = functionalities.begin();
        for(; cit != functionalities.end(); ++cit)
        {
            const Functionality& functionality = *cit;
            const owlapi::model::IRI& functionalityModel = functionality.getModel();
            // Requesting from the functionality mapping will in most cases
            // embed the functional saturation bound, e.g.
            // the minimum to provide a certain functionality (per se, without
            // accouting for a particular dataProperty requirement)
            //
            // In principle one will need a special join function for each kind
            // of data property -- right now we assume that we can 'sum' the
            // dataproperties in order to deal with the requirements
            //
            try {
                // Computing all model combination that support this
                // functionality
                FunctionalityRequirement::Map::const_iterator cit = functionalityRequirements.find(functionality);
                ModelPool::Set modelPoolSet;
                if(cit == functionalityRequirements.end())
                {
                    // no constraints need to be considered
                    modelPoolSet = mFunctionalityMapping.getModelPools(functionalityModel);
                } else {
                    // deal with additional constraints
                    modelPoolSet = getResourceSupport(cit->second);
                }

                supportingCompositions = Algebra::maxCompositions(supportingCompositions, modelPoolSet);
            } catch(const std::invalid_argument& e)
            {
                LOG_DEBUG_S << "Could not find resource support for service: '" << functionalityModel;
                return ModelPool::Set();
            }
        }
    }
    return supportingCompositions;
}

ModelPool::Set OrganizationModelAsk::getResourceSupport(const Functionality::Set& functionalities) const
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
    ModelPool::Set supportingCompositions;
    {
        Functionality::Set::const_iterator cit = functionalities.begin();
        for(; cit != functionalities.end(); ++cit)
        {
            const Functionality& functionality = *cit;
            const owlapi::model::IRI& functionalityModel = functionality.getModel();
            try {
                 ModelPool::Set modelPoolSet = mFunctionalityMapping.getModelPools(functionalityModel);
                 supportingCompositions = Algebra::maxCompositions(supportingCompositions, modelPoolSet);
            } catch(const std::invalid_argument& e)
            {
                LOG_DEBUG_S << "Could not find resource support for service: '" << functionalityModel;
                return ModelPool::Set();
            }
        }
    }
    return supportingCompositions;
}

ModelPool::Set OrganizationModelAsk::getBoundedResourceSupport(const Functionality::Set& functionalities) const
{
    ModelPool::Set modelPools = getResourceSupport(functionalities);
    modelPools = filterNonMinimal(modelPools, functionalities);
    ModelPool bound = getFunctionalSaturationBound(functionalities);
    return ModelPool::applyUpperBound(modelPools, bound);
}

ModelCombinationSet OrganizationModelAsk::applyUpperBound(const ModelCombinationSet& combinations, const ModelPool& upperBound) const
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

ModelPool::Set OrganizationModelAsk::applyLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBound) const
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

ModelPool::Set OrganizationModelAsk::expandToLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBound) const
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

algebra::SupportType OrganizationModelAsk::getSupportType(const Functionality::Set& functionalities, const owlapi::model::IRI& model, uint32_t cardinalityOfModel) const
{
    ModelPool modelPool;
    modelPool[model] = cardinalityOfModel;
    return getSupportType(functionalities, modelPool);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const Functionality& functionality, const owlapi::model::IRI& model, uint32_t cardinalityOfModel) const
{
    Functionality::Set functionalities;
    functionalities.insert(functionality);
    return getSupportType(functionalities, model, cardinalityOfModel);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const Functionality::Set& functionalities, const ModelPool& modelPool) const
{
    IRIList functionalityModels;
    Functionality::Set::const_iterator fit = functionalities.begin();
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
    Functionality::Set functionalities;
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

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const Functionality::Set& functionalities) const
{
    ModelPool upperBounds;
    Functionality::Set::const_iterator cit = functionalities.begin();
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

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const Functionality::Set& functionalities, const FunctionalityRequirement& constraints) const
{
    ModelPool modelSupport = getFunctionalSaturationBound(functionalities);
    for(ModelPool::value_type& pair : modelSupport)
    {
        ModelPool m;
        m.insert(pair);
        try {
            double p = getScalingFactor(m, constraints);
            // the functional saturation bound
            pair.second = p;
        } catch(const std::invalid_argument& e)
        {
            // does not contribute towards a solution
            pair.second = 0;
        }
    }

    return modelSupport;
}

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const Functionality::Set& functionalities, const FunctionalityRequirement::Map& constraints) const
{
    ModelPool upperBounds;
    for(const FunctionalityRequirement::Map::value_type& v : constraints)
    {
        const FunctionalityRequirement& constraint = v.second;
        ModelPool saturation = getFunctionalSaturationBound(functionalities, constraint);

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

bool OrganizationModelAsk::isSupporting(const ModelPool& modelPool, const Functionality::Set& functionalities) const
{
    // Requires the functionality mapping to be properly initialized

    Functionality::Set::const_iterator cit = functionalities.begin();
    ModelPool::Set previousModelPools;
    bool init = true;
    for(; cit != functionalities.end(); ++cit)
    {
        const Functionality& functionality = *cit;
        try {
            const ModelPool::Set& modelPools = mFunctionalityMapping.getModelPools(functionality.getModel());

            if(init)
            {
                previousModelPools = modelPools;
                init = false;
                continue;
            }

            ModelPool::Set resultList;
            std::set_intersection(modelPools.begin(), modelPools.end(),
                    previousModelPools.begin(), previousModelPools.end(),
                    std::inserter(resultList, resultList.begin()) );
             previousModelPools.insert(resultList.begin(), resultList.end());

        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << "Could not find functionality: " << functionality.getModel().toString() << std::endl
                << modelPool.toString(4) << std::endl
                << "current functionality mappping: " << std::endl
                << mFunctionalityMapping.toString(4);

            throw std::runtime_error("organization_model::OrganizationModelAsk::isSupporting"
                    " could not find functionality '" + functionality.getModel().toString() + "'");
        }

    }

    ModelPool::Set::const_iterator pit = std::find(previousModelPools.begin(), previousModelPools.end(), modelPool);
    if(pit != previousModelPools.end())
    {
        return true;
    } else {
        return false;
    }
}

bool OrganizationModelAsk::isSupporting(const ModelPool& pool, const Functionality& functionality) const
{
    Functionality::Set functionalities;
    functionalities.insert(functionality);
    return isSupporting(pool, functionalities);
}

bool OrganizationModelAsk::isSupporting(const owlapi::model::IRI& model, const Functionality& functionality) const
{
    ModelPool modelPool;
    modelPool.setResourceCount(model, 1);

    Functionality::Set functionalities;
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
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = mOntologyAsk.getCardinalityRestrictions(models, vocabulary::OM::has(), OWLCardinalityRestriction::MAX_OP);

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


std::vector<double> OrganizationModelAsk::getScalingFactors(const ModelPool::Set& modelPoolSet, const FunctionalityRequirement& functionalityRequirement, bool doCheckSupport) const
{
    std::vector<double> scalingFactors(modelPoolSet.size(), 1.0);
    size_t index = 0;
    for(const ModelPool& pool : modelPoolSet)
    {
        try {
            double scalingFactor = getScalingFactor(pool, functionalityRequirement, doCheckSupport);
            updateScalingFactor(scalingFactors, index, scalingFactor);
        } catch(const std::runtime_error& e)
        {
            // Model pool is not supported
            updateScalingFactor(scalingFactors, index, 0);
        }
        ++index;
    }
    return scalingFactors;
}

double OrganizationModelAsk::getScalingFactor(const ModelPool& modelPool, const FunctionalityRequirement& functionalityRequirement, bool doCheckSupport) const
{
    // assuming that model supporting
    if(doCheckSupport)
    {
        if(!isSupporting(modelPool, functionalityRequirement.getFunctionality()))
        {
            return 0.0;
        }
    }

    const PropertyConstraint::Set& constraints = functionalityRequirement.getPropertyConstraints();
    PropertyConstraint::Clusters clusteredConstraints = PropertyConstraint::getClusters(constraints);

    double scalingFactor = 1.0;
    std::map<owlapi::model::IRI, double> propertyValue;
    std::map<owlapi::model::IRI, ValueBound> valueBoundFactors;

    // Find the global scaling factor, which is defined by the set of
    // requirements
    for(const PropertyConstraint::Clusters::value_type property2Constraints : clusteredConstraints)
    {
        const owlapi::model::IRI& property = property2Constraints.first;
        double value;
        try {
            // the existing data property value
            value = getDataPropertyValue(modelPool, property);
            propertyValue[property] = value;

            ValueBound vb;
            // Will throw when the constraints cannot be fulfilled
            vb = PropertyConstraintSolver::merge(property2Constraints.second);

            if(scalingFactor*value >= vb.getMin())
            {
                // existing scalingFactor is sufficient, e.g to reach the minimum
                scalingFactor = std::max(1.0, scalingFactor);
            } else {
                // in order to reach the minimum: compute the scaling factor
                double minFactor = std::ceil( vb.getMin() / value );
                scalingFactor = std::max(minFactor, scalingFactor);
            }
        } catch(const std::runtime_error& e)
        {
            throw std::invalid_argument("organization_model::getScalingFactor: functional requirement cannot be fulfilled by this model pool: " + modelPool.toString(12) + " - " + e.what());
        }

    }

    for(const PropertyConstraint::Clusters::value_type property2Constraints : clusteredConstraints)
    {
        const owlapi::model::IRI& property = property2Constraints.first;
        // the existing data property value
        double scaledValue = scalingFactor*propertyValue[property];
        const ValueBound& vb = valueBoundFactors[property];

        if( scaledValue < vb.getMin() || scaledValue > vb.getMax())
        {
            throw std::invalid_argument("organization_model::getScalingFactor: functional requirement cannot be fulfilled by this model pool: " + modelPool.toString(12) +
                    " a feasible scaling factor could not be found for all requirements");
        }
    }
    return scalingFactor;
}

void OrganizationModelAsk::updateScalingFactor(std::vector<double>& factors, size_t idx, double newValue)
{
    double& v = factors.at(idx);
    if(v == 0)
    {
        return;
    } else if(newValue == 0)
    {
        v = 0;
        return;
    } else {
        v = std::max(v, newValue);
    }
}

} // end namespace organization_model
