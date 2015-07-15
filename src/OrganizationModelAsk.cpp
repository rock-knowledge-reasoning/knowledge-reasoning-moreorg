#include "OrganizationModelAsk.hpp"
#include "Algebra.hpp"
#include <sstream>

#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/csp/ResourceMatch.hpp>
#include <base/Time.hpp>
#include <numeric/LimitedCombination.hpp>
#include <owlapi/Vocabulary.hpp>

#include <base/Logging.hpp>

using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace organization_model {

OrganizationModelAsk::OrganizationModelAsk(OrganizationModel::Ptr om, const ModelPool& modelPool)
    : mpOrganizationModel(om)
    , mOntologyAsk(om->ontology())
{
    if(!modelPool.empty())
    {
        prepare(modelPool);
    } else {
        LOG_WARN_S << "No model pool provided: did not prepare functionality mappings";
    }
}

void OrganizationModelAsk::prepare(const ModelPool& modelPool)
{
    mModelPool = modelPool;
    mFunctionalityMapping = getFunctionalityMapping(mModelPool);
}

owlapi::model::IRIList OrganizationModelAsk::getServiceModels() const
{
    bool directSubclassOnly = false;
    IRIList subclasses = mOntologyAsk.allSubclassesOf(OM::Service(), directSubclassOnly);
    return subclasses;
}

FunctionalityMapping OrganizationModelAsk::getFunctionalityMapping(const ModelPool& modelPool) const
{
    FunctionalityMapping functionalityMapping;

    if(modelPool.empty())
    {
        throw std::invalid_argument("organization_model::OrganizationModel::getFunctionalityMaps"
                " cannot compute functionality map for empty model pool");
    }

    numeric::LimitedCombination<owlapi::model::IRI> limitedCombination(modelPool,
            numeric::LimitedCombination<owlapi::model::IRI>::totalNumberOfAtoms(modelPool), numeric::MAX);

    IRIList serviceModels = getServiceModels();
    std::pair<Combination2FunctionMap, Function2CombinationMap> functionalityMaps;

    uint32_t count = 0;
    do {
        // Get the current model combination
        IRIList combination = limitedCombination.current();
        // Make sure we have a consistent ordering
        std::sort(combination.begin(), combination.end());

        LOG_WARN_S << "Check combination #" << ++count;
        LOG_WARN_S << "   | --> combination:             " << combination;
        LOG_WARN_S << "   | --> possible service models: " << serviceModels;

        base::Time startTime = base::Time::now();
        // Filter the serviceModel (from the existing set) which are supported
        // by this very combination
        IRIList supportedServiceModels = owlapi::csp::ResourceMatch::filterSupportedModels(combination, serviceModels, mpOrganizationModel->ontology());

        base::Time stopTime = base::Time::now();
        LOG_WARN_S << "   | --> required time: " << (stopTime - startTime).toSeconds();
        // Update the mapping functions - forward and inverse mapping from
        // model/combination to function
        functionalityMapping.combination2Function[combination] = supportedServiceModels;
        IRIList::const_iterator cit = supportedServiceModels.begin();
        for(; cit != supportedServiceModels.end(); ++cit)
        {
            IRI iri = *cit;
            functionalityMapping.function2Combination[iri].insert(combination);
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
            Function2CombinationMap::const_iterator fit = mFunctionalityMapping.function2Combination.find(serviceModel);
            if(fit != mFunctionalityMapping.function2Combination.end())
            {
                serviceProviders[serviceModel] = fit->second;
            } else {
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
    ModelCombination boundedCombination = OrganizationModel::modelPool2Combination(upperBound);
    std::sort(boundedCombination.begin(), boundedCombination.end());

    ModelCombinationSet::const_iterator cit = combinations.begin();
    for(; cit != combinations.end(); ++cit)
    {
        ModelCombination combination = *cit;
        std::sort(combination.begin(), combination.end());

        if( std::includes(boundedCombination.begin(), boundedCombination.end(),
            combination.begin(), combination.end()) )
        {
            LOG_DEBUG_S << "Includes" << std::endl
                << "bounded: " << boundedCombination << std::endl
                << "tested: " << combination << std::endl;
            boundedCombinations.insert(combination);
        }
    }

    LOG_DEBUG_S << "Bound set of resources: " << std::endl
        << "prev: " << OrganizationModel::toString(combinations) << std::endl
        << "bound: " << ModelPoolDelta(upperBound).toString() << std::endl
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
    algebra::ResourceSupportVector serviceSupportVector = getSupportVector(service.getModel(), IRIList(), false /*useMaxCardinality*/);
    algebra::ResourceSupportVector modelSupportVector = getSupportVector(model, serviceSupportVector.getLabels(), true /*useMaxCardinality*/);

    serviceSupportVector = serviceSupportVector.embedClassRelationship(*this);
    modelSupportVector = modelSupportVector.embedClassRelationship(*this);

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
        Function2CombinationMap::const_iterator cit = mFunctionalityMapping.function2Combination.find(service.getModel());
        if(cit != mFunctionalityMapping.function2Combination.end())
        {
            throw std::runtime_error("organization_model::OrganizationModelAsk::isSupporting \
                    could not find service '" + service.getModel().toString() + "'");
        }

        const ModelCombinationSet& combination = cit->second;
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
    }

    ModelCombinationSet::const_iterator pit = std::find(previousCombinations.begin(), previousCombinations.end(), c);
    if(pit != previousCombinations.end())
    {
        return true;
    } else {
        return false;
    }
}

algebra::ResourceSupportVector OrganizationModelAsk::getSupportVector(const owlapi::model::IRI& model,
        const owlapi::model::IRIList& filterLabels,
        bool useMaxCardinality) const
{
    using namespace owlapi::model;
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = mOntologyAsk.getCardinalityRestrictions(model);
    std::map<IRI, OWLCardinalityRestriction::MinMax> modelCount = OWLCardinalityRestriction::getBounds(restrictions);
    return getSupportVector(modelCount, filterLabels, useMaxCardinality);

}

algebra::ResourceSupportVector OrganizationModelAsk::getSupportVector(const std::map<owlapi::model::IRI,
    owlapi::model::OWLCardinalityRestriction::MinMax>& modelBounds,
    const owlapi::model::IRIList& filterLabels, bool useMaxCardinality) const
{
    using namespace owlapi::model;

    base::VectorXd vector;
    std::vector<IRI> labels;

    if(filterLabels.empty())
    {
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
        vector = base::VectorXd::Zero(filterLabels.size());
        labels = filterLabels;

        std::vector<IRI>::const_iterator cit = filterLabels.begin();
        uint32_t dimension = 0;
        for(; cit != filterLabels.end(); ++cit)
        {
            const IRI& dimensionLabel = *cit;
            std::map<IRI, OWLCardinalityRestriction::MinMax>::const_iterator mit = modelBounds.begin();
            for(; mit != modelBounds.end(); ++mit)
            {
                const IRI& modelDimensionLabel = mit->first;
                LOG_DEBUG_S << "Check model support for " << dimensionLabel << "  from " << modelDimensionLabel;

                // Sum the requirement/availability of this model type
                if(dimensionLabel == modelDimensionLabel || mOntologyAsk.isSubclassOf(modelDimensionLabel, dimensionLabel))
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
