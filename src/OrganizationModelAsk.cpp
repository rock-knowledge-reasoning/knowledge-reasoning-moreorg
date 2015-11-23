#include "OrganizationModelAsk.hpp"
#include "Algebra.hpp"
#include <sstream>
#include <base/Logging.hpp>
#include <base/Time.hpp>
#include <numeric/LimitedCombination.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/Vocabulary.hpp>
#include <organization_model/reasoning/ResourceMatch.hpp>
#include <organization_model/vocabularies/OM.hpp>


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
            LOG_WARN_S << "No functional saturation bound requested: this might take some time to prepare the functionality mappings";
        }
        prepare(modelPool, mApplyFunctionalSaturationBound);
    } else {
        LOG_INFO_S << "No model pool provided: did not prepare functionality mappings";
    }
}

void OrganizationModelAsk::prepare(const ModelPool& modelPool, bool applyFunctionalSaturationBound)
{
    mModelPool = modelPool;
    mFunctionalityMapping = getFunctionalityMapping(mModelPool, applyFunctionalSaturationBound);
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

FunctionalityMapping OrganizationModelAsk::getFunctionalityMapping(const ModelPool& modelPool, bool applyFunctionalSaturationBound) const
{
    if(modelPool.empty())
    {
        throw std::invalid_argument("organization_model::OrganizationModel::getFunctionalityMaps"
                " cannot compute functionality map for empty model pool");
    }

    // TODO: rename serviceModels --> functionalities
    IRIList serviceModels = getFunctionalities();
    std::pair<Combination2FunctionMap, Function2CombinationMap> functionalityMaps;

    ModelPool functionalSaturationBound;
    // TODO: create an bound on the model pool based on the given service
    // models based on the FunctionalSaturation -- avoids computing unnecessary
    // combination as function mappings
    //
    // LocationImageProvider: CREX --> 1, Sherpa --> 1
    if(applyFunctionalSaturationBound)
    {
        // Compute service set of all known services
        ServiceSet services;
        {
            IRIList::const_iterator cit = serviceModels.begin();
            for(; cit != serviceModels.end(); ++cit)
            {
                const IRI& serviceModel = *cit;
                services.insert( Service(serviceModel) );
            }
        }

        // Compute the bound for all services
        LOG_INFO_S << "Get functional saturation bound for '" << serviceModels;
        functionalSaturationBound = getFunctionalSaturationBound(services);
        LOG_INFO_S << "Functional saturation bound for '" << serviceModels << "' is "
            << functionalSaturationBound.toString();

        // Merge with the existing model pool
        functionalSaturationBound = modelPool.applyUpperBound(functionalSaturationBound);
        LOG_INFO_S << "After accounting for given model pool the functional saturation bound for '" << serviceModels << "' is "
            << functionalSaturationBound.toString();
    } else {
        LOG_WARN_S << "No functional saturation bound applied";
        functionalSaturationBound = modelPool;
    }

    if(modelPool.empty())
    {
        throw std::runtime_error("organization_model::OrganizationModelAsk::getFunctionalityMapping: provided model pool empty");
    } else if(serviceModels.empty())
    {
        throw std::runtime_error("organization_model::OrganizationModelAsk::getFunctionalityMapping: available services empty");
    } else if(functionalSaturationBound.empty())
    {
        std::string msg = "organization_model::OrganizationModelAsk::getFunctionalityMapping: provided empty functionalSaturationBound";
        msg += modelPool.toString() + "\n";
        msg += owlapi::model::IRI::toString(serviceModels) + "\n";
        msg += functionalSaturationBound.toString() + "\n";
        throw std::runtime_error(msg);
    }

    FunctionalityMapping functionalityMapping(modelPool, serviceModels, functionalSaturationBound);

    const ModelPool& boundedModelPool = functionalityMapping.getFunctionalSaturationBound();
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

        LOG_INFO_S << "Check combination #" << ++count;
        LOG_INFO_S << "   | --> combination:             " << combination;
        LOG_INFO_S << "   | --> possible service models: " << serviceModels;

        base::Time startTime = base::Time::now();
        // Filter the serviceModel (from the existing set) which are supported
        // by this very combination
        IRIList supportedServiceModels = reasoning::ResourceMatch::filterSupportedModels(combination, serviceModels, mpOrganizationModel->ontology());

        base::Time stopTime = base::Time::now();
        LOG_INFO_S << "   | --> required time: " << (stopTime - startTime).toSeconds();
        LOG_INFO_S << "Update";
        {
            base::Time startTime = base::Time::now();
            // Update the mapping functions - forward and inverse mapping from
            // model/combination to function
            functionalityMapping.add(combination, supportedServiceModels);
            base::Time stopTime = base::Time::now();
            LOG_INFO_S << "   | --> required time: " << (stopTime - startTime).toSeconds();
        }
    } while(limitedCombination.next());

    return functionalityMapping;
}

ModelCombinationSet OrganizationModelAsk::getResourceSupport(const ServiceSet& services) const
{
    if(services.empty())
    {
        std::invalid_argument("organization_model::OrganizationModelAsk::getResourceSupport:"
                " no service given in request");
    }

    /// Store the systems that support the functionality
    /// i.e. per requested function the combination of models that support it,
    LOG_DEBUG_S << "FunctionalityMap: " << mFunctionalityMapping.toString();
    Function2CombinationMap serviceProviders;
    {
        ServiceSet::const_iterator cit = services.begin();
        for(; cit != services.end(); ++cit)
        {
            const Service& service = *cit;
            const owlapi::model::IRI& serviceModel =  service.getModel();
            try {
                serviceProviders[serviceModel] = mFunctionalityMapping.getCombinations(serviceModel);
            } catch(const std::invalid_argument& e)
            {
                LOG_DEBUG_S << "Could not find resource support for service: '" << serviceModel;
                return ModelCombinationSet();
            }
        }
    }
    LOG_DEBUG_S << "Found service providers: " << OrganizationModel::toString(serviceProviders);

    // Only requested one service
    if(services.size() == 1)
    {
        return serviceProviders.begin()->second;
    }

    // If looking for a combined system that can provide all the services
    // requested, then the intersection of the sects is the solution of this
    // request
    bool init = true;
    std::set<ModelCombination> resultSet;
    std::set<ModelCombination> lastResult;
    Function2CombinationMap::const_iterator cit = serviceProviders.begin();
    for(; cit != serviceProviders.end(); ++cit)
    {
        const std::set<ModelCombination>& currentSet = cit->second;
        if(init)
        {
            resultSet = currentSet;
            init = false;
            continue;
        } else {
            lastResult = resultSet;
        }

        resultSet.clear();
        LOG_DEBUG_S << "Intersection: current set: " << OrganizationModel::toString(currentSet);
        LOG_DEBUG_S << "Intersection: last set: " << OrganizationModel::toString(lastResult);
        std::set_intersection(currentSet.begin(), currentSet.end(), lastResult.begin(), lastResult.end(),
                std::inserter(resultSet, resultSet.begin()));
        LOG_DEBUG_S << "Intersection: result" << OrganizationModel::toString(resultSet);
    }

    return resultSet;

    //// Use ModelCombination here in order to generate
    //std::set<ModelCombination> resources;
    //{
    //    // Iterate over all service providers
    //    // 1. check if existing service provider provides the current service
    //    //   -- if not then check if resources are sufficient to provide both
    //    //   --   if so add to the updated resource list
    //    Function2CombinationMap::const_iterator cit = serviceProviders.begin();
    //    for(; cit != serviceProviders.end(); ++cit)
    //    {
    //        const Service& service = cit->first;
    //        // Combinations that support the given service
    //        const ModelCombinationList& combinationList = cit->second;

    //        LOG_DEBUG_S << "Checking service: " << service.getModel();

    //        if(resources.empty())
    //        {
    //            ModelCombinationList::const_iterator combinationIt = combinationList.begin();
    //            for(; combinationIt != combinationList.end(); ++combinationIt)
    //            {
    //                resources.insert(*combinationIt);
    //            }
    //            continue;
    //        } else {

    //            std::set<ModelCombination> updatedResources;

    //            ModelCombinationList::const_iterator combinationIt = combinationList.begin();
    //            for(; combinationIt != combinationList.end(); ++combinationIt)
    //            {
    //                const ModelCombination& combination = *combinationIt;
    //                LOG_DEBUG_S << "ModelCombination: " << IRI::toString(combination, true);

    //                std::set<ModelCombination>::const_iterator mit = resources.begin();
    //                for(; mit != resources.end(); ++mit)
    //                {
    //                    ModelPool requirements = Algebra::merge(*mit, combination);
    //                    ModelPoolDelta delta = Algebra::delta(requirements, mModelPool);
    //                    LOG_DEBUG_S << "Existing " << ModelPoolDelta(mModelPool).toString();
    //                    LOG_DEBUG_S << "Requirements " << ModelPoolDelta(requirements).toString();
    //                    LOG_DEBUG_S << "Delta " << delta.toString();

    //                    if( delta.isNegative() )
    //                    {
    //                        // not enough resources
    //                        LOG_DEBUG_S << "Not enough resources";
    //                    } else {
    //                        updatedResources.insert(
    //                    }
    //                }
    //            }
    //            resources = updatedResources;
    //        }
    //    }
    //}
    //return resources;


    ///// Go through the set of combinations and find valid
    ///// assignments, i.e. where
    ///// (a) one model combination supports all services -->
    /////     intersection of function combination lists
    ///// (b) individual but possibly distinct combinations support services

    //std::vector<ModelCombinationList> resources;
    //{
    //    bool first = true;
    //    ModelCombinationList previousCombinationList;
    //    // (a) Individual systems that support all functions
    //    Function2CombinationMap::const_iterator cit = resultMap.begin();
    //    for(; cit != resultMap.end(); ++cit)
    //    {
    //        if(first)
    //        {
    //            previousCombinationList = cit->second;
    //            continue;
    //        }

    //        ModelCombinationList currentCombinationList = cit->second;

    //        ModelCombinationList resultList(previousCombinationList.size() + currentCombinationList.size());

    //        ModelCombinationList::iterator it;
    //        it = std::set_intersection(previousCombinationList.begin(),
    //                previousCombinationList.end(),
    //                currentCombinationList.begin(),
    //                currentCombinationList.end(), resultList.begin());

    //        resultList.resize(it - resultList.begin());
    //        previousCombinationList = resultList;
    //    }

    //    ModelCombinationList::const_iterator mit = previousCombinationList.begin();
    //    for(; mit != previousCombinationList.end(); ++mit)
    //    {
    //        // One list per system
    //        ModelCombinationList combination;
    //        combination.push_back(*mit);
    //        resources.push_back(combination);
    //    }
    //}


    //return resources;
}

ModelCombinationSet OrganizationModelAsk::getBoundedResourceSupport(const ServiceSet& services) const
{
    ModelCombinationSet combinations = getResourceSupport(services);
    ModelPool bound = getFunctionalSaturationBound(services);
    return applyUpperBound(combinations, bound);
}

ModelCombinationSet OrganizationModelAsk::applyUpperBound(const ModelCombinationSet& combinations, const ModelPool& upperBound) const
{
    ModelCombinationSet boundedCombinations;
    ModelCombinationSet::const_iterator cit = combinations.begin();
    for(; cit != combinations.end(); ++cit)
    {
        ModelPool modelPool = OrganizationModel::combination2ModelPool(*cit);

        LOG_DEBUG_S << "DELTA lva: " << ModelPoolDelta(upperBound).toString();
        LOG_DEBUG_S << "DELTA rval: " << ModelPoolDelta(modelPool).toString();
        ModelPoolDelta delta = Algebra::substract(modelPool, upperBound);
        LOG_DEBUG_S << "RESULT: " << delta.toString();
        if(!delta.isNegative())
        {
            boundedCombinations.insert(*cit);
        }
    }

    LOG_DEBUG_S << "Upper bound set on resources: " << std::endl
        << "prev: " << OrganizationModel::toString(combinations) << std::endl
        << "bound: " << ModelPoolDelta(upperBound).toString() << std::endl
        << "bounded: " << OrganizationModel::toString(boundedCombinations);
    return boundedCombinations;
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
        << "bound: " << ModelPoolDelta(lowerBound).toString() << std::endl
        << "bounded: " << OrganizationModel::toString(boundedCombinations);
    return boundedCombinations;
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
        << "bound: " << ModelPoolDelta(lowerBound).toString() << std::endl
        << "bounded: " << OrganizationModel::toString(boundedCombinations);
    return boundedCombinations;
}


//std::set<ModelCombination> OrganizationModelAsk::getMinimalResourceSupport(const ServiceSet& services) const
//{
//
//    ModelPool upperBound = getFunctionalSaturationBound(services);
//    FuncationalityMapping functionalityMapping = getFunctionalityMappings(upperBound);
//
//
//
//}
//
//std::set<ModelCombination> OrganizationModelAsk::getMinimalResourceSupport_v1(const ServiceSet& services) const
//{
//    std::set<ModelCombination> modelCombinationSet;
//    std::set<ModelCombinationSet> modelSet = getResourceSupport(services);
//    std::set<ModelCombinationSet>::const_iterator cit = modelSet.begin();
//    for(; cit != modelSet.end(); ++cit)
//    {
//        const ModelCombinationSet& combinations = *cit;
//        ModelCombinationSet::const_iterator mit = combinations.begin();
//
//        ModelPoolDelta delta;
//        bool init = true;
//
//        for(; mit != combinations.end(); ++mit)
//        {
//            if(init)
//            {
//                delta = OrganizationModel::combination2ModelPool(*mit);
//                init = false;
//            } else {
//                delta = Algebra::sum( OrganizationModel::combination2ModelPool(*mit), delta );
//            }
//        }
//
//        ModelPool pool = delta.toModelPool();
//        modelCombinationSet.insert( OrganizationModel::modelPool2Combination(pool) );
//    }
//    return modelCombinationSet;
//}

algebra::SupportType OrganizationModelAsk::getSupportType(const Service& service, const owlapi::model::IRI& model, uint32_t cardinalityOfModel) const
{
    algebra::ResourceSupportVector serviceSupportVector = getSupportVector(service.getModel(), IRIList(), false /*useMaxCardinality*/);
    algebra::ResourceSupportVector modelSupportVector =
        getSupportVector(model, serviceSupportVector.getLabels(), true /*useMaxCardinality*/)*
        static_cast<double>(cardinalityOfModel);

    return serviceSupportVector.getSupportFrom(modelSupportVector, *this);
}

uint32_t OrganizationModelAsk::getFunctionalSaturationBound(const Service& service, const owlapi::model::IRI& model) const
{
    LOG_DEBUG_S << "Get functional saturation bound for " << service.getModel() << " for model '" << model << "'";
    // Collect requirements, i.e., max cardinalities
    algebra::ResourceSupportVector serviceSupportVector = getSupportVector(service.getModel(), IRIList(), false /*useMaxCardinality*/);
    if(serviceSupportVector.isNull())
    {
        owlapi::model::IRIList labels;
        labels.push_back(service.getModel());
        base::VectorXd required(1);
        required(0) = 1;
        serviceSupportVector = algebra::ResourceSupportVector(required, labels);
        LOG_DEBUG_S << "functionality support vector is null : using " << serviceSupportVector.toString();
    } else {
        LOG_DEBUG_S << "Get model support vector";
    }
    // Collect available resources -- and limit to the required ones
    // (getSupportVector will accumulate all (subclass) models)
    algebra::ResourceSupportVector modelSupportVector = getSupportVector(model, serviceSupportVector.getLabels(), true /*useMaxCardinality*/);
    LOG_DEBUG_S << "Retrieved model support vector";

    // Expand the support vectors to account for subclasses within the required
    // scope
    serviceSupportVector = serviceSupportVector.embedClassRelationship(*this);
    modelSupportVector = modelSupportVector.embedClassRelationship(*this);

    // Compute the support ratios
    algebra::ResourceSupportVector ratios = serviceSupportVector.getRatios(modelSupportVector);

    LOG_DEBUG_S << "Service: " << serviceSupportVector.toString();
    LOG_DEBUG_S << "Provider: " << modelSupportVector.toString();
    LOG_DEBUG_S << "Ratios: " << ratios.toString();

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

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const Service& service) const
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
        uint32_t saturation = getFunctionalSaturationBound(service, cit->first);
        upperBounds[cit->first] = saturation;
    }
    return upperBounds;
}

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const ServiceSet& services) const
{
    ModelPool upperBounds;
    ServiceSet::const_iterator cit = services.begin();
    for(; cit != services.end(); ++cit)
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

//std::set<ModelCombination> OrganizationModelAsk::getMinimalResourceSupport_v2(const ServiceSet& services) const
//{
//    std::set<ModelCombination> modelCombinations;
//    std::vector<owlapi::model::IRI> models = ModelPoolDelta::getModels(mModelPool);
//
//    // Check 'type-clean' service support
//    std::vector<owlapi::model::IRI>::const_iterator cit = models.begin();
//    for(; cit != models.end(); ++cit)
//    {
//        try {
//            int32_t minCardinality = minRequiredCardinality(services, *cit);
//            ModelPool modelPool;
//            modelPool[*cit] = minCardinality;
//
//            modelCombinations.insert( OrganizationModel::modelPool2Combination(modelPool) );
//        } catch(const std::runtime_error& e)
//        {
//            LOG_DEBUG_S << e.what();
//        }
//    }
//    return modelCombinations;
//}
//
//uint32_t OrganizationModelAsk::minRequiredCardinality(const ServiceSet& services, const owlapi::model::IRI& model) const
//{
//    uint32_t lower = 1;
//    ModelPool::const_iterator cit = mModelPool.find(model);
//    if(cit != mModelPool.end())
//    {
//        throw std::invalid_argument("organization_model::OrganizationModelAsk::minRequiredCardinality: could not find model '" + model.toString() + "' in pool");
//    }
//
//    uint32_t upper = cit->second;
//
//    bool supportExists = false;
//
//    uint32_t currentPosition;
//    do
//    {
//        currentPosition = static_cast<uint32_t>( (lower + upper) / 2.0 );
//
//        ModelCombination combination;
//        for(uint32_t i = 0; i < currentPosition; ++i)
//        {
//            combination.push_back(model);
//        }
//
//        if( isSupporting(combination, services) )
//        {
//            upper = currentPosition;
//            supportExists = true;
//        } else {
//            lower = currentPosition + 1;
//        }
//    } while(currentPosition < upper);
//
//    if(!supportExists)
//    {
//        throw std::runtime_error("OrganizationModelAsk::minRequired no cardinality of model '" + model.toString() + "' can provide the given set of services");
//    }
//
//    return currentPosition;
//}

bool OrganizationModelAsk::canBeDistinct(const ModelCombination& a, const ModelCombination& b) const
{
    ModelPool poolA = OrganizationModel::combination2ModelPool(a);
    ModelPool poolB = OrganizationModel::combination2ModelPool(b);

    ModelPoolDelta totalRequirements = Algebra::sum(poolA, poolB);
    ModelPoolDelta delta = Algebra::delta(totalRequirements, mModelPool);

    return delta.isNegative();
}

bool OrganizationModelAsk::isSupporting(const ModelCombination& c, const ServiceSet& services) const
{
    ServiceSet::const_iterator cit = services.begin();
    ModelCombinationSet previousCombinations;
    bool init = true;
    for(; cit != services.end(); ++cit)
    {
        const Service& service = *cit;
        try {
            const ModelCombinationSet& combination = mFunctionalityMapping.getCombinations(service.getModel());

            if(init)
            {
                previousCombinations = combination;
                init = false;
                continue;
            }

            ModelCombinationSet resultList;
            std::set_intersection(combination.begin(), combination.end(),
                    previousCombinations.begin(), previousCombinations.end(),
                    std::inserter(resultList, resultList.begin()) );
             previousCombinations.insert(resultList.begin(), resultList.end());

        } catch(const std::invalid_argument& e)
        {
            throw std::runtime_error("organization_model::OrganizationModelAsk::isSupporting \
                    could not find service '" + service.getModel().toString() + "'");
        }

    }

    ModelCombinationSet::const_iterator pit = std::find(previousCombinations.begin(), previousCombinations.end(), c);
    if(pit != previousCombinations.end())
    {
        return true;
    } else {
        return false;
    }
}

bool OrganizationModelAsk::isSupporting(const owlapi::model::IRI& model, const Service& serviceModel) const
{
    ModelPool modelPool;
    modelPool.setResourceCount(model, 1);

    ServiceSet services;
    services.insert(serviceModel);
    if( isSupporting(modelPool.toModelCombination(), services) )
    {
        LOG_DEBUG_S << "model '" << model << "' supports '" << serviceModel.getModel() << "'";
        return true;
    } else {
        LOG_DEBUG_S << "model '" << model << "' does not support '" << serviceModel.getModel() << "'";
        return false;
    }  
}

algebra::ResourceSupportVector OrganizationModelAsk::getSupportVector(const owlapi::model::IRI& model,
        const owlapi::model::IRIList& filterLabels,
        bool useMaxCardinality) const
{
    using namespace owlapi::model;
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = mOntologyAsk.getCardinalityRestrictions(model);
    if(restrictions.empty())
    {
        return algebra::ResourceSupportVector();
    } else {
        std::map<IRI, OWLCardinalityRestriction::MinMax> modelCount = OWLCardinalityRestriction::getBounds(restrictions);
        LOG_WARN_S << "ModelCount: "<< modelCount.size() << ", restrictions: " << restrictions.size();
        if(restrictions.size() == 1)
        {
            LOG_WARN_S << restrictions[0]->toString();
        }
        return getSupportVector(modelCount, filterLabels, useMaxCardinality);
    }

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

    LOG_DEBUG_S << "Get support vector" << std::endl
        << "    " << vector << std::endl
        << "    " << labels;

    return algebra::ResourceSupportVector(vector, labels);
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
