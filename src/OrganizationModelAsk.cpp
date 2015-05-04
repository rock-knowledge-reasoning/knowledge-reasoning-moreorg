#include "OrganizationModelAsk.hpp"
#include "Algebra.hpp"
#include <sstream>

#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/csp/ResourceMatch.hpp>
#include <base/Time.hpp>
#include <numeric/LimitedCombination.hpp>
#include <owlapi/Vocabulary.hpp>

using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace organization_model {

OrganizationModelAsk::OrganizationModelAsk(OrganizationModel::Ptr om, const ModelPool& modelPool)
    : mpOrganizationModel(om)
    , mModelPool(modelPool)
{
    prepare();
}

void OrganizationModelAsk::prepare()
{
    computeFunctionalityMaps(mModelPool);
}

owlapi::model::IRIList OrganizationModelAsk::getServiceModels() const
{
    bool directSubclassOnly = false;
    IRIList subclasses = mpOrganizationModel->ask()->allSubclassesOf(OM::Service(), directSubclassOnly);
    return subclasses;
}

void OrganizationModelAsk::computeFunctionalityMaps(const ModelPool& modelPool)
{
    numeric::LimitedCombination<owlapi::model::IRI> limitedCombination(modelPool, 
            numeric::LimitedCombination<owlapi::model::IRI>::totalNumberOfAtoms(modelPool), numeric::MAX);

    mCombination2Function.clear();
    mFunction2Combination.clear();

    IRIList serviceModels = getServiceModels();

    uint32_t count = 0;
    do {
        // Get the current model combination
        IRIList combination = limitedCombination.current();
        // Make sure we have a consistent ordering
        std::sort(combination.begin(), combination.end());

        LOG_WARN_S << "Check combination #" << ++count;
        LOG_WARN_S << "    --> combination:             " << combination;
        LOG_WARN_S << "    --> possible service models: " << serviceModels;

        base::Time startTime = base::Time::now();
        // Filter the serviceModel (from the existing set) which are supported
        // by this very combination
        IRIList supportedServiceModels = owlapi::csp::ResourceMatch::filterSupportedModels(combination, serviceModels, mpOrganizationModel->ontology());

        base::Time stopTime = base::Time::now();
        LOG_WARN_S << "    --> required time: " << (stopTime - startTime).toSeconds();
        // Update the mapping functions - forward and inverse mapping from
        // model/combination to function
        mCombination2Function[combination] = supportedServiceModels;
        IRIList::const_iterator cit = supportedServiceModels.begin();
        for(; cit != supportedServiceModels.end(); ++cit)
        {
            IRI iri = *cit;
            mFunction2Combination[iri].push_back(combination);
        }
    } while(limitedCombination.next());
}

std::set<ModelCombinationSet> OrganizationModelAsk::getResourceSupport(const ServiceSet& services) const
{
    /// Store the systems that support the functionality
    /// i.e. per requested function the combination of models that support it,
    Function2CombinationMap serviceProviders;
    {
        ServiceSet::const_iterator cit = services.begin();
        for(; cit != services.end(); ++cit)
        {
            const Service& service = *cit;
            const owlapi::model::IRI& serviceModel =  service.getModel();
            Function2CombinationMap::const_iterator fit = mFunction2Combination.find(serviceModel);
            if(fit != mFunction2Combination.end())
            {
                serviceProviders[serviceModel] = fit->second;
            } else {
                LOG_DEBUG_S << "Could not find resource support for service: '" << serviceModel;
                return std::set<ModelCombinationSet>();
            }
        }
    }

    std::set<ModelCombinationSet> resources;
    {
        // Iterate over all service providers
        // 1. check if existing service provider provides the current service
        //   -- if not check if resources are sufficient to provide both
        //   --   if so add to the updated resource list 
        bool init = true;
        Function2CombinationMap::const_iterator cit = serviceProviders.begin();
        for(; cit != serviceProviders.end(); ++cit)
        {
            const Service& service = cit->first;
            const ModelCombinationList& combinationList = cit->second;

            LOG_DEBUG_S << "Checking service: " << service.getModel();

            if(init)
            {
                ModelCombinationList::const_iterator combinationIt = combinationList.begin();
                for(; combinationIt != combinationList.end(); ++combinationIt)
                {
                    ModelCombinationSet set;
                    set.insert(*combinationIt);
                    resources.insert(set);
                }
                init = false;
                continue;
            } else {

                std::set<ModelCombinationSet> updatedResourceList;
                ModelCombinationList::const_iterator combinationIt = combinationList.begin();
                for(; combinationIt != combinationList.end(); ++combinationIt)
                {
                    const ModelCombination& combination = *combinationIt;
                    LOG_DEBUG_S << "ModelCombination: " << IRI::toString(combination, true);

                    std::set<ModelCombinationSet>::const_iterator mit = resources.begin();
                    for(; mit != resources.end(); ++mit)
                    {
                        const ModelCombinationSet& combinationSet = *mit;

                        ModelPool requirements = Algebra::merge(combinationSet, combination);
                        ModelPoolDelta delta = Algebra::delta(requirements, mModelPool);
                        LOG_DEBUG_S << "Existing " << ModelPoolDelta(mModelPool).toString();
                        LOG_DEBUG_S << "Requirements " << ModelPoolDelta(requirements).toString();
                        LOG_DEBUG_S << "Delta " << delta.toString();

                        if( delta.isNegative() )
                        {
                            // not enough resources
                            LOG_DEBUG_S << "Not enough resources";
                        } else {
                            std::set< ModelCombination > updatedCombinationSet = combinationSet;
                            LOG_DEBUG_S << "Enough resources";
                            updatedCombinationSet.insert(combination);

                            updatedResourceList.insert( updatedCombinationSet );
                        }
                    }
                }
                resources = updatedResourceList;
            }
        }
    }
    return resources;


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


std::set<ModelCombination> OrganizationModelAsk::getMinimalResourceSupport(const ServiceSet& services) const
{
    std::set<ModelCombination> modelCombinationSet;
    std::set<ModelCombinationSet> modelSet = getResourceSupport(services);
    std::set<ModelCombinationSet>::const_iterator cit = modelSet.begin();
    for(; cit != modelSet.end(); ++cit)
    {
        const ModelCombinationSet& combinations = *cit;
        ModelCombinationSet::const_iterator mit = combinations.begin();

        ModelPoolDelta delta;
        bool init = true;

        for(; mit != combinations.end(); ++mit)
        {
            if(init)
            {
                delta = OrganizationModel::combination2ModelPool(*mit);
                init = false;
            } else {
                delta = Algebra::sum( OrganizationModel::combination2ModelPool(*mit), delta );
            }
        }

        ModelPool pool = delta.toModelPool();
        modelCombinationSet.insert( OrganizationModel::modelPool2Combination(pool) );
    }
    return modelCombinationSet;
}

bool OrganizationModelAsk::canBeDistinct(const ModelCombination& a, const ModelCombination& b)
{
    ModelPool poolA = OrganizationModel::combination2ModelPool(a);
    ModelPool poolB = OrganizationModel::combination2ModelPool(b);

    ModelPoolDelta totalRequirements = Algebra::sum(poolA, poolB);
    ModelPoolDelta delta = Algebra::delta(totalRequirements, mModelPool);

    return delta.isNegative();
}

} // end namespace organization_model
