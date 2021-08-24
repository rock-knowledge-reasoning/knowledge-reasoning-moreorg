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
#include "PropertyConstraintSolver.hpp"
#include "utils/OrganizationStructureGeneration.hpp"
#include <unordered_map>
#include <fstream>


using namespace owlapi::model;
using namespace owlapi::vocabulary;

namespace moreorg {

std::vector<OrganizationModelAsk> OrganizationModelAsk::msOrganizationModelAsk;

OrganizationModelAsk::OrganizationModelAsk()
    : mOntologyAsk( OWLOntology::Ptr() )
{}

OrganizationModelAsk::OrganizationModelAsk(const OrganizationModel::Ptr& om,
        const ModelPool& modelPool,
        bool applyFunctionalSaturationBound,
        double feasibilityCheckTimeoutInMs,
        const owlapi::model::IRI& interfaceBaseClass,
        size_t neighbourHood
        )
    : mpOrganizationModel(om)
    , mOntologyAsk(om->ontology())
    , mApplyFunctionalSaturationBound(applyFunctionalSaturationBound)
    , mFeasibilityCheckTimeoutInMs(feasibilityCheckTimeoutInMs)
    , mStructuralNeighbourhood(neighbourHood)
    , mInterfaceBaseClass(interfaceBaseClass)
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

const OrganizationModelAsk& OrganizationModelAsk::getInstance(const OrganizationModel::Ptr& om,
        const ModelPool& modelPool,
        bool applyFunctionalSaturationBound,
        double feasibilityCheckTimeoutInMs,
        const owlapi::model::IRI& interfaceBaseClass,
        size_t neighbourHood
        )
{
    std::vector<OrganizationModelAsk>::const_iterator it = std::find_if(msOrganizationModelAsk.begin(), msOrganizationModelAsk.end(),
            [om,modelPool,applyFunctionalSaturationBound,feasibilityCheckTimeoutInMs,
            interfaceBaseClass](const OrganizationModelAsk& other)
            {
                if(om == other.mpOrganizationModel
                        && applyFunctionalSaturationBound == other.mApplyFunctionalSaturationBound
                        && feasibilityCheckTimeoutInMs == other.mFeasibilityCheckTimeoutInMs
                        && modelPool == other.mModelPool
                        && interfaceBaseClass == other.mInterfaceBaseClass)
                {
                    return true;
                } else {
                    return false;
                }
            });
    if(it != msOrganizationModelAsk.end())
    {
        return *it;
    } else {
        OrganizationModelAsk ask(om,
                modelPool, applyFunctionalSaturationBound,
                feasibilityCheckTimeoutInMs, interfaceBaseClass, neighbourHood);
        msOrganizationModelAsk.push_back(ask);
        return msOrganizationModelAsk.back();
    }
}

void OrganizationModelAsk::prepare(const ModelPool& modelPool, bool applyFunctionalSaturationBound)
{
    mModelPool = allowSubclasses(modelPool, vocabulary::OM::Actor());
    mModelPool = mModelPool.compact();
    mFunctionalityMapping = computeFunctionalityMapping(mModelPool, applyFunctionalSaturationBound);
}

owlapi::model::IRIList OrganizationModelAsk::getAgentModels() const
{
    bool directSubclassOnly = false;
    IRIList subclasses = mOntologyAsk.allSubClassesOf(vocabulary::OM::Agent(),
            directSubclassOnly);
    return subclasses;
}

owlapi::model::IRIList OrganizationModelAsk::getAgentProperties(const IRI& model) const
{
    return mOntologyAsk.getDataPropertiesForDomain(model);
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
    IRIList blacklist = { vocabulary::OM::Service(), vocabulary::OM::Capability() };
    for(const owlapi::model::IRI& label : blacklist)
    {
        IRIList::iterator it = std::find_if(subclasses.begin(), subclasses.end(), [label](const owlapi::model::IRI& subclass)
                {
                    return subclass == label;
                });
        subclasses.erase(it);
    }

    return subclasses;
}

owlapi::model::IRIList OrganizationModelAsk::getSupportedFunctionalities(const ModelPool& model) const
{
    return mFunctionalityMapping.getFunctionalities(model);
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
        LOG_WARN_S << "moreorg::OrganizationModelAsk::computeFunctionalityMapping"
                " cannot compute functionality map for empty model pool";
        return FunctionalityMapping();
    }

    IRIList functionalityModels = getFunctionalities();
    if(functionalityModels.empty())
    {
        throw std::runtime_error("moreorg::OrganizationModelAsk::computeFunctionalityMapping: available functionalities empty");
    }

    FunctionalityMapping functionalityMapping;

    size_t hashPool = std::hash<std::string>{}(modelPool.toString(0));
    std::stringstream ss;
    ss << "/tmp/moreorg-om-cache-" << hashPool;
    if(applyFunctionalSaturationBound)
    {
        ss << "-with-sat-bound";
    }
    std::string cacheFilename = ss.str();

    std::ifstream cacheFile(cacheFilename);
    if(cacheFile.is_open())
    {
        cacheFile.close();

        functionalityMapping = FunctionalityMapping::fromFile(cacheFilename);
        return functionalityMapping;
    }

    if(applyFunctionalSaturationBound)
    {
        functionalityMapping = computeBoundedFunctionalityMapping(modelPool, functionalityModels);
    } else {
        functionalityMapping = computeUnboundedFunctionalityMapping(modelPool, functionalityModels);
    }

    functionalityMapping.save(cacheFilename);

    return functionalityMapping;
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
    Resource::Set functionalities = Resource::toResourceSet(functionalityModels);

    // Compute the bound for the combination of all known services
    LOG_DEBUG_S << "Get functional saturation bound for '" << functionalityModels;
    ModelPool functionalSaturationBound = getFunctionalSaturationBound(functionalities);
    LOG_DEBUG_S << "Functional saturation bound for '" << functionalityModels << "' is "
        << functionalSaturationBound.toString();

    // Apply bound to the existing model pool - thus creating the global upper
    // bound
    functionalSaturationBound = modelPool.applyUpperBound(functionalSaturationBound);
    LOG_INFO_S << "Model pool after applying the functional saturation bound for '" << functionalityModels << "' is "
        << functionalSaturationBound.toString();

    if(functionalSaturationBound.empty())
    {
        std::string msg = "moreorg::OrganizationModelAsk::computeBoundedFunctionalityMapping: provided empty functionalSaturationBound";
        msg += modelPool.toString() + "\n";
        msg += owlapi::model::IRI::toString(functionalityModels) + "\n";
        msg += functionalSaturationBound.toString() + "\n";
        throw std::runtime_error(msg);
    }

    // Apply for each functionality individually the functional saturation bound
    FunctionalityMapping functionalityMapping(modelPool, functionalityModels, functionalSaturationBound);
    Resource::Set::const_iterator fit = functionalities.begin();
    for(; fit != functionalities.end(); ++fit)
    {
        const Resource& functionality = *fit;
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
        size_t count = 0;
        do {
            IRIList combination = limitedCombination.current();
            ModelPool combinationModelPool = OrganizationModel::combination2ModelPool(combination);

            LOG_INFO_S << "CHECK COMBINATION: " << count++ << std::endl
                << combinationModelPool.toString(4);
            base::Time start = base::Time::now();
            bool isFeasiblePool = isFeasible(combinationModelPool);

            base::Time end = base::Time::now();
            if(isFeasiblePool)
            {
                LOG_INFO_S << "Is feasible: " << (end-start).toSeconds();
            } else {
                LOG_INFO_S << "Is not feasible" << (end-start).toSeconds();
            }

            // identify the potential additions
            ModelPool explorePool;
            if(mStructuralNeighbourhood > 0)
            {
                // Handle a bound that represents only structurally infeasible systems
                if(!combinationModelPool.isNull() && !isFeasible(combinationModelPool))
                {
                    for(const ModelPool::value_type v :  mModelPool)
                    {
                        size_t currentModelCardinality = boundedModelPool[v.first];
                        if( currentModelCardinality == 0 && v.second > 0)
                        {
                            explorePool[v.first] = v.second;
                        } else {
                            // check for types that are funtionally bounded
                            // (but can still contribute structurally)
                            size_t remaining = v.second - currentModelCardinality;
                            if(remaining > 0)
                            {
                                explorePool[v.first] = remaining;
                            }
                        }
                    }
                }
            }

            exploreNeighbourhood(functionalityMapping,
                    combinationModelPool,
                    explorePool,
                    functionality.getModel(),
                    mStructuralNeighbourhood);
        } while(limitedCombination.next());

    } // end for functionalities

    return functionalityMapping;
}

bool OrganizationModelAsk::addFunctionalityMapping(FunctionalityMapping& functionalityMapping,
        const ModelPool& combinationModelPool,
        const owlapi::model::IRI& functionality,
        bool minimalOnly) const
{
    Resource::Set functionalities;
    functionalities.insert(functionality);
    if(minimalOnly && !isMinimal(combinationModelPool, functionalities))
    {
        LOG_DEBUG_S << "combination is not minimal for " << functionality.toString() << std::endl
            << combinationModelPool.toString(4);
        return false;
    } else {
        LOG_DEBUG_S << "combination is minimal for " << functionality.toString() << std::endl
            << combinationModelPool.toString(4);
        if(algebra::Connectivity::isFeasible(combinationModelPool,
                    *this,
                    mFeasibilityCheckTimeoutInMs,
                    1, // minFeasible
                    mInterfaceBaseClass))
        {
            LOG_DEBUG_S << "combination is feasible " << std::endl
            << combinationModelPool.toString(4);
            functionalityMapping.add(combinationModelPool, functionality);
        }
        return true;
    }
}

FunctionalityMapping OrganizationModelAsk::computeUnboundedFunctionalityMapping(const ModelPool& modelPool,
        const IRIList& functionalityModels) const
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

        LOG_DEBUG_S << "Check combination #" << ++count << std::endl
                 << "   | --> combination:             " << combination << std::endl
                 << "   | --> possible functionality models: " << functionalityModels << std::endl;

        ModelPool combinationModelPool = OrganizationModel::combination2ModelPool(combination);

        // system that already provides full support for this functionality
        owlapi::model::IRIList::const_iterator cit = functionalityModels.begin();
        for(; cit != functionalityModels.end(); ++cit)
        {
            Resource functionality(*cit);
            algebra::SupportType supportType = getSupportType(functionality, combinationModelPool);
            if(algebra::FULL_SUPPORT == supportType)
            {
                if( !addFunctionalityMapping(functionalityMapping,
                            combinationModelPool,
                            functionality.getModel(), false) )
                {
                    LOG_DEBUG_S << "Failed to add to functionality mapping:" << std::endl
                        << "    functionality: " << functionality.getModel().toString() << std::endl
                        << "    combination: \n" << combinationModelPool.toString(8) << std::endl;
                }
            }
        }
    } while(limitedCombination.next());

    return functionalityMapping;
}


bool OrganizationModelAsk::isMinimal(const ModelPool& modelPool, const Resource::Set& functionalities) const
{
    // Check overall support
    algebra::SupportType supportType = getSupportType(functionalities, modelPool);
    if(algebra::FULL_SUPPORT != supportType)
    {
        LOG_INFO_S << "No full support for " << Resource::toString(functionalities) << " by " << std::endl << modelPool.toString(4);
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
            << "    " << Resource::toString(functionalities);

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
            << "    " << Resource::toString(functionalities);

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

double OrganizationModelAsk::getPropertyValue(const ModelPool& modelPool,
        const owlapi::model::IRI& property) const
{
    facades::Robot robot = facades::Robot::getInstance(modelPool, *this);
    return robot.getPropertyValue(property);
}

std::vector<OWLCardinalityRestriction::Ptr> OrganizationModelAsk::getRequiredCardinalities(
        const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty) const
{
    ModelPool agents;
    ModelPool functionalities;
    for(const ModelPool::value_type& m : modelPool)
    {
        if(ontology().isSubClassOf(m.first, vocabulary::OM::Agent()))
        {
            agents[m.first] = m.second;
        } else if(ontology().isSubClassOf(m.first, vocabulary::OM::Functionality()))
        {
            agents[m.first] = m.second;
        }
    }

    std::vector<OWLCardinalityRestriction::Ptr> f_required = getCardinalityRestrictions(functionalities, objectProperty, OWLCardinalityRestriction::SUM_OP);
    std::vector<OWLCardinalityRestriction::Ptr> r_required = getCardinalityRestrictions(agents, objectProperty, OWLCardinalityRestriction::SUM_OP, true);

    std::vector<OWLCardinalityRestriction::Ptr> required = OWLCardinalityRestrictionOps::join(f_required, r_required, OWLCardinalityRestriction::MIN_OP);

    required.erase( std::remove_if(required.begin(), required.end(), [](const OWLCardinalityRestriction::Ptr& elem)
            {
                return elem->getCardinalityRestrictionType() == OWLObjectCardinalityRestriction::MAX;
            }), required.end()
    );
    return required;
}

std::vector<OWLCardinalityRestriction::Ptr> OrganizationModelAsk::getAvailableCardinalities(
        const ModelPool& modelPool,
        const owlapi::model::IRI& objectProperty) const
{
    // available cardinalities can only be computed for agents
    ModelPool availableAgents = allowSubclasses(modelPool, vocabulary::OM::Actor());

    std::vector<OWLCardinalityRestriction::Ptr> available =
        getCardinalityRestrictions(availableAgents, objectProperty,
                OWLCardinalityRestriction::SUM_OP);
    available.erase( std::remove_if(available.begin(), available.end(), [](const OWLCardinalityRestriction::Ptr& elem)
            {
                return elem->getCardinalityRestrictionType() == OWLObjectCardinalityRestriction::MIN;
            }), available.end()
    );
    return available;
}

std::vector<owlapi::model::OWLCardinalityRestriction::Ptr> OrganizationModelAsk::getCardinalityRestrictions(const ModelPool& modelPool,
    const owlapi::model::IRI& objectProperty,
    owlapi::model::OWLCardinalityRestriction::OperationType operationType,
    bool max2Min) const
{
    std::pair<owlapi::model::OWLCardinalityRestriction::PtrList, bool> result =
        mpOrganizationModel->mQueryCache.getCachedResult(modelPool, objectProperty,
            operationType, max2Min);

    if(result.second)
    {
        return result.first;
    }

    std::vector<OWLCardinalityRestriction::Ptr> allAvailableResources;

    owlapi::model::OWLProperty::Ptr property = ontology().getOWLObjectProperty(objectProperty);
    // Get model restrictions, i.e. in effect what has to be available for the
    // given models
    ModelPool::const_iterator mit = modelPool.begin();
    for(; mit != modelPool.end(); ++mit)
    {
        const IRI& model = mit->first;
        uint32_t modelCount = mit->second;

        OWLCardinalityRestriction::PtrList availableResources =
            mOntologyAsk.getCardinalityRestrictions(model, objectProperty,
                    true /*includeAncestors*/);

        // This is not a meta constraint, but a direct representation of an
        // atomic resource, so add the exact availability of the given high level resource, which is
        // defined through the model pool
        if(availableResources.empty())
        {
            OWLClassExpression::Ptr klass =
                mOntologyAsk.getOWLClassExpression(mit->first);

            OWLCardinalityRestriction::Ptr highlevelModelMaxConstraint = OWLCardinalityRestriction::getInstance(property,
                    modelCount,
                    klass,
                    OWLCardinalityRestriction::MAX);
            availableResources.push_back(highlevelModelMaxConstraint);

            OWLCardinalityRestriction::Ptr highlevelModelMinConstraint = OWLCardinalityRestriction::getInstance(property,
                    modelCount,
                    klass,
                    OWLCardinalityRestriction::MIN);
            availableResources.push_back(highlevelModelMinConstraint);
        }

        std::vector<OWLCardinalityRestriction::Ptr> available;
        for(const OWLCardinalityRestriction::Ptr& r : availableResources)
        {
            OWLObjectCardinalityRestriction::Ptr restriction =
                dynamic_pointer_cast<OWLObjectCardinalityRestriction>(r);
            if(!restriction)
            {
                throw
                    std::runtime_error("moreorg::OrganizationModelAsk::getCardinalityRestrictions:"
                        " expected OWLObjectCardinalityRestriction");
            }

            // Update the cardinality with the actual model count
            uint32_t cardinality = modelCount*restriction->getCardinality();
            if(max2Min && restriction->getCardinalityRestrictionType() == OWLCardinalityRestriction::MAX)
            {
                OWLClassExpression::Ptr klass =
                    mOntologyAsk.getOWLClassExpression(restriction->getQualification());

                OWLCardinalityRestriction::Ptr converted = OWLCardinalityRestriction::getInstance(restriction->getProperty(),
                        cardinality,
                        klass,
                        OWLCardinalityRestriction::MIN);
                available.push_back(converted);
            } else {
                OWLCardinalityRestriction::Ptr r = restriction->clone();
                r->setCardinality(cardinality);
                available.push_back(r);
            }
        }

        allAvailableResources = OWLCardinalityRestrictionOps::join(allAvailableResources, available, operationType );
    }

    mpOrganizationModel->mQueryCache.cacheResult(modelPool, objectProperty,
            operationType,
            max2Min,
            allAvailableResources);

    return allAvailableResources;
}

ModelPool::Set OrganizationModelAsk::filterNonMinimal(const ModelPool::Set& modelPoolSet, const Resource::Set& functionalities) const
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

ModelPool::Set OrganizationModelAsk::getResourceSupport(const Resource& resource) const
{
    const owlapi::model::IRI& functionalityModel = resource.getModel();

    // If there are no property constraints
    if(resource.getPropertyConstraints().empty())
    {
        return mFunctionalityMapping.getModelPools(functionalityModel);
    }

    try {
        // Computing all model combination that support this functionality
        ModelPool::Set modelPoolSet = mFunctionalityMapping.getModelPools(functionalityModel);
        ModelPool::Set supportPool;

        std::vector<double> scalingFactors = getScalingFactors(modelPoolSet, resource);

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
            LOG_WARN_S << "moreorg::OrganizationModelAsk::getResourceSupport: could not compute functionality constrained model pool, since model pool is missing for setting the upper bound";
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

ModelPool::Set OrganizationModelAsk::getResourceSupport(const Resource::Set& resources) const
{
    ModelPool::Set supportingCompositions;
    for(const Resource& resource : resources)
    {
        // Requesting from the functionality mapping will in most cases
        // embed the functional saturation bound, e.g.
        // the minimum to provide a certain functionality (per se, without
        // accounting for a particular dataProperty requirement)
        //
        // In principle one will need a special join function for each kind
        // of data property -- right now we assume that we can 'sum' the
        // dataproperties in order to deal with the requirements
        //
        try {
            // deal with additional constraints
            ModelPool::Set modelPoolSet = getResourceSupport(resource);
            supportingCompositions = Algebra::maxCompositions(supportingCompositions, modelPoolSet);
        } catch(const std::invalid_argument& e)
        {
            LOG_DEBUG_S << "Could not find support for resource: '" << resource.getModel() << "'";
            return ModelPool::Set();
        }
    }

    return supportingCompositions;
}

ModelPool::Set OrganizationModelAsk::getBoundedResourceSupport(const Resource::Set& functionalities) const
{
    ModelPool::Set modelPools = getResourceSupport(functionalities);
    modelPools = filterNonMinimal(modelPools, functionalities);
    ModelPool bound = getFunctionalSaturationBound(functionalities);
    return ModelPool::applyUpperBound(modelPools, bound);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const Resource::Set& functionalities,
        const owlapi::model::IRI& model,
        uint32_t cardinalityOfModel) const
{
    ModelPool modelPool;
    modelPool[model] = cardinalityOfModel;
    return getSupportType(functionalities, modelPool);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const Resource& functionality,
        const owlapi::model::IRI& model,
        uint32_t cardinalityOfModel) const
{
    Resource::Set functionalities;
    functionalities.insert(functionality);
    return getSupportType(functionalities, model, cardinalityOfModel);
}

algebra::SupportType OrganizationModelAsk::getSupportType(const Resource::Set& functionalities,
        const ModelPool& modelPool) const
{
    IRIList functionalityModels;
    Resource::Set::const_iterator fit = functionalities.begin();
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

algebra::SupportType OrganizationModelAsk::getSupportType(const Resource& functionality,
        const ModelPool& modelPool) const
{
    Resource::Set functionalities;
    functionalities.insert(functionality);
    return getSupportType(functionalities, modelPool);
}

uint32_t OrganizationModelAsk::getFunctionalSaturationBound(const owlapi::model::IRI& requirementModel,
        const owlapi::model::IRI& model) const
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

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const Resource& resource) const
{
    if(mModelPool.empty())
    {
        throw std::invalid_argument("moreorg::OrganizationModelAsk::getFunctionalSaturationBound:"
                " model pool is empty. Call OrganizationModelAsk::prepare with model pool");
    }

    ModelPool upperBounds;
    for(const ModelPool::value_type& pair : mModelPool)
    {
        const owlapi::model::IRI& modelName = pair.first;

        uint32_t saturation = getFunctionalSaturationBound(resource.getModel(),
                modelName);
        upperBounds[modelName] = saturation;

        if(resource.getPropertyConstraints().empty())
        {
            continue;
        } else {
            // Account for any given constraint
            ModelPool m;
            m.insert( ModelPool::value_type(modelName, saturation) );
            try {
                double p = getScalingFactor(m, resource);
                // the functional saturation bound
                upperBounds[modelName] = p;
            } catch(const std::invalid_argument& e)
            {
                // does not contribute towards a solution
                upperBounds[modelName] = 0;
            }
        }
    }
    return upperBounds;
}

ModelPool OrganizationModelAsk::getFunctionalSaturationBound(const Resource::Set& functionalities) const
{
    ModelPool upperBounds;
    Resource::Set::const_iterator cit = functionalities.begin();
    for(; cit != functionalities.end(); ++cit)
    {
        // Return the functional saturation for this functionality
        ModelPool saturation = getFunctionalSaturationBound(*cit);

        // the maximum required value for a functionality sets the functional
        // saturation bound
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

ModelPool::Set OrganizationModelAsk::getIntersection(const Resource::Set& functionalities) const
{
    // Requires the functionality mapping to be properly initialized
    Resource::Set::const_iterator cit = functionalities.begin();
    ModelPool::Set previousModelPools;
    bool init = true;
    for(; cit != functionalities.end(); ++cit)
    {
        const Resource& functionality = *cit;
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
                << "current functionality mappping: " << std::endl
                << mFunctionalityMapping.toString(4);

            throw std::runtime_error("moreorg::OrganizationModelAsk::isSupporting"
                    " could not find functionality '" +
                    functionality.getModel().toString() + "' -- " + e.what());
        }
    }
    return previousModelPools;
}

bool OrganizationModelAsk::isSupporting(const ModelPool& modelPool,
        const Resource::Set& resources,
        double feasibilityCheckTimeoutInMs) const
{

    ModelPool::Set supportPools = getIntersection(resources);

    // Any of the support models is assumed to be a minimal subset, e.g. with
    // functional saturation
    //
    // TODO: this does not yet account for any negative effects, i.e., when the
    // coalition is large and for example mass constraints prevent it from
    // functioning -- find a general way for representation:
    // by sat bound limited agents + negative effects
    ModelPool::Set::const_iterator pit = std::find_if(supportPools.begin(), supportPools.end(),
            [modelPool](const ModelPool& other)
            {
                // check if other is a subset of the given model pool
                return Algebra::isSubset(other, modelPool);
            });

    if(pit != supportPools.end())
    {
        // what is left to be checked is whether this pool is actually feasible
        return algebra::Connectivity::isFeasible(modelPool, *this,
                feasibilityCheckTimeoutInMs,
                1, // minFeasible
                mInterfaceBaseClass);
    } else {
        return false;
    }
}

bool OrganizationModelAsk::isSupporting(const ModelPool& pool,
        const Resource& resource) const
{
    Resource::Set resources;
    resources.insert(resource);
    return isSupporting(pool, resources,
            mFeasibilityCheckTimeoutInMs);
}

bool OrganizationModelAsk::isSupporting(const owlapi::model::IRI& model,
        const Resource& resource) const
{
    ModelPool modelPool;
    modelPool.setResourceCount(model, 1);

    Resource::Set resources;
    resources.insert(resource);
    if( isSupporting(modelPool, resources,
                mFeasibilityCheckTimeoutInMs
                ) )
    {
        LOG_DEBUG_S << "model '" << model << "' supports '" << resource.getModel() << "'";
        return true;
    } else {
        LOG_DEBUG_S << "model '" << model << "' does not support '" << resource.getModel() << "'";
        return false;
    }
}


algebra::ResourceSupportVector OrganizationModelAsk::getSupportVector(const owlapi::model::IRIList& models,
        const owlapi::model::IRIList& filterLabels,
        bool useMaxCardinality) const
{
    using namespace owlapi::model;
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = mOntologyAsk.getCardinalityRestrictions(models, vocabulary::OM::has(), OWLCardinalityRestriction::MAX_OP);

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
        std::map<IRI, OWLCardinalityRestriction::MinMax> modelCount = OWLObjectCardinalityRestriction::getBounds(restrictions);
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
        throw std::invalid_argument("moreorg::OrganizationModelAsk::getSupportVector: no model bounds given");
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

    vector = base::VectorXd::Zero(labels.size());

    std::vector<IRI>::const_iterator cit = labels.begin();
    uint32_t dimension = 0;
    for(; cit != labels.end(); ++cit)
    {
        const IRI& dimensionLabel = *cit;
        std::map<IRI, OWLCardinalityRestriction::MinMax>::const_iterator mit = modelBounds.begin();
        bool supportFound = false;
        for(; mit != modelBounds.end(); ++mit)
        {
            const IRI& modelDimensionLabel = mit->first;
            // Sum the requirement/availability of this model type
            if(dimensionLabel == modelDimensionLabel || mOntologyAsk.isSubClassOf(modelDimensionLabel, dimensionLabel))
            {
                if(useMaxCardinality)
                {
                    // using max value
                    vector(dimension) += mit->second.second;
                    supportFound = true;
                } else {
                    // using min value
                    vector(dimension) += mit->second.first;
                    supportFound = true;
                }
            }
        }
        if(supportFound)
        {
            LOG_INFO_S << "Found support for model: " << dimensionLabel
                << " with cardinality '" << vector(dimension) << "'";
        } else {
            LOG_INFO_S << "Found no support for model: " << dimensionLabel;
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

std::vector<double> OrganizationModelAsk::getScalingFactors(const ModelPool::Set& modelPoolSet,
        const Resource& resource,
        bool doCheckSupport) const
{
    std::vector<double> scalingFactors(modelPoolSet.size(), 1.0);
    size_t index = 0;
    for(const ModelPool& pool : modelPoolSet)
    {
        try {
            double scalingFactor = getScalingFactor(pool, resource, doCheckSupport);
            updateScalingFactor(scalingFactors, index, scalingFactor);
        } catch(const std::exception& e)
        {
            // Model pool is not supported
            updateScalingFactor(scalingFactors, index, 0);
        }
        ++index;
    }
    return scalingFactors;
}

double OrganizationModelAsk::getScalingFactor(const ModelPool& modelPool,
        const Resource& resource,
        bool doCheckSupport) const
{
    // Either assume that model is supporting the particular resource (e.g. functionality)
    // , or perform a dedicated check
    if(doCheckSupport)
    {
        if(!isSupporting(modelPool, resource))
        {
            return 0.0;
        }
    }

    const PropertyConstraint::Set& constraints = resource.getPropertyConstraints();
    // Set of property constraint set, where each set is referring to a
    // particular property
    PropertyConstraint::Clusters clusteredConstraints = PropertyConstraint::getClusters(constraints);

    std::map<owlapi::model::IRI, double> propertyValue;

    // Find the global scaling factor, which is defined by the set of
    // requirements
    double minScalingFactor = 1.0;
    size_t maxResourceCount = modelPool.getMaxResourceCount();
    double maxScalingFactor = maxResourceCount;

    for(const PropertyConstraint::Clusters::value_type property2Constraints : clusteredConstraints)
    {
        const owlapi::model::IRI& property = property2Constraints.first;
        try {
            // the existing data property value with respect to the cardinality constraint
            double value = getPropertyValue(modelPool, property);
            propertyValue[property] = value;

            // Merge the existing clustered constraint into a single consistent
            // one
            // Will throw when the constraints cannot be fulfilled
            ValueBound vb = PropertyConstraintSolver::merge(property2Constraints.second);

            // Min scaling
            // If current min scaling factor is too small
            if(minScalingFactor*value < vb.getMin())
            {
                // in order to reach the minimum: compute the scaling factor
                double minFactor = std::ceil( vb.getMin() / value );
                // increase min factor
                minScalingFactor = std::max(minFactor, minScalingFactor);
            }

            if(minScalingFactor*value > vb.getMax())
            {
                throw
                    std::runtime_error("Min scaling factor exceeds the allowed"
                            "maximum value");
            }

            // Max scaling
            // If current scaling factor is too large
            if(maxScalingFactor*value > vb.getMax())
            {
                // in order to remain within the maximum: compute the scaling factor
                double maxFactor = std::floor( vb.getMax() / value );
                maxScalingFactor = std::min(maxFactor, maxScalingFactor);
            }

            if(maxScalingFactor < minScalingFactor)
            {
                maxScalingFactor = minScalingFactor;
            }
        } catch(const std::exception& e)
        {
            throw std::invalid_argument("moreorg::getScalingFactor: functional requirement cannot be fulfilled by this model pool: " + modelPool.toString(12) + " - " + e.what());
        }
    }
    return minScalingFactor;
}

void OrganizationModelAsk::updateScalingFactor(std::vector<double>& factors, size_t idx, double newValue)
{
    double& v = factors.at(idx);
    if(v == 0)
    {
        // functionality cannot be supported at all
        return;
    } else if(newValue == 0)
    {
        v = 0;
        // functionality cannot be supported at all
        return;
    } else {
        // functionality can ony be supported with scaling of given
        // maximum value
        v = std::max(v, newValue);
    }
}


void OrganizationModelAsk::exploreNeighbourhood(FunctionalityMapping& functionalityMapping,
        const ModelPool& basePool,
        const ModelPool& maxDelta,
        const IRI& functionality,
        size_t maxAddedInstances) const
{
    size_t numberOfAtoms =
        numeric::LimitedCombination<owlapi::model::IRI>::totalNumberOfAtoms(maxDelta);
    if(numberOfAtoms > maxAddedInstances)
    {
        // Maximum # of indirection for connecting or consideration of
        // additional components to achieve functionality (when structurally
        // inconsistent)
        numberOfAtoms = maxAddedInstances;
    }

    if(numberOfAtoms == 0 || maxAddedInstances == 0)
    {
        // just check the provided base pool
        if( !addFunctionalityMapping(functionalityMapping,
                basePool, functionality) )
        {
            LOG_DEBUG_S << "Failed to add to functionality mapping:" << std::endl
                << "    functionality: " << functionality.toString() << std::endl
                << "    combination: " << basePool.toString(4) << std::endl
                << "    neighbourhoodsize: " << maxAddedInstances << std::endl;
        }
        return;
    }

    // Explore the neighbourhood, but limiting the compositions in size
    numeric::LimitedCombination<owlapi::model::IRI> limitedCombination(maxDelta, numberOfAtoms, numeric::MAX);
    do {
        IRIList combination = limitedCombination.current();
        ModelPool combinationModelPool = OrganizationModel::combination2ModelPool(combination);
        ModelPool pool = Algebra::sum(basePool, combinationModelPool).toModelPool();

        if( !addFunctionalityMapping(functionalityMapping,
                pool, functionality) )
        {
            LOG_DEBUG_S << "Failed to add to functionality mapping during"
                << " neighborhood search: "<< std::endl
                << "    functionality: " << functionality.toString()
                << "    combination: " << combinationModelPool.toString(4);
                combinationModelPool.toString(4);
        }
    } while(limitedCombination.next());
}

ModelPool OrganizationModelAsk::allowSubclasses(const ModelPool& modelPool,
        const owlapi::model::IRI& parent) const
{
    ModelPool filteredModelPool;
    for(const ModelPool::value_type p : modelPool)
    {
        if( mOntologyAsk.isSubClassOf(p.first, parent) )
        {
            filteredModelPool.insert(p);
        }
    }
    return filteredModelPool;
}

bool OrganizationModelAsk::isFeasible(const ModelPool& modelPool,
        double feasibilityCheckTimeoutInMs
        ) const
{
    return algebra::Connectivity::isFeasible(modelPool, *this,
            feasibilityCheckTimeoutInMs,
            1, // minFeasible
            mInterfaceBaseClass
            );
}

ModelPool::List OrganizationModelAsk::findFeasibleCoalitionStructure(const ModelPool& modelPool,
        const Resource::Set& resourceSet,
        double feasibilityCheckTimeoutInMs
        )
{
    std::pair<ModelPool::List, bool> result = mpOrganizationModel->mQueryCache.getCachedResult(modelPool, resourceSet);
    if(result.second)
    {
        return result.first;
    }

    AtomicAgent::List agents = AtomicAgent::toList(modelPool);
    utils::CoalitionStructureGeneration csg(agents,
            [this, resourceSet, feasibilityCheckTimeoutInMs](const AtomicAgent::List& agents) -> double
            {
                ModelPool pool = AtomicAgent::getModelPool(agents);
                if(isSupporting(pool, resourceSet,
                            feasibilityCheckTimeoutInMs))

                {
                    return 1.0;
                }
                return 0.0;
            },
            [this, resourceSet, feasibilityCheckTimeoutInMs](const std::vector<AtomicAgent::List>& csg) -> double
            {
                for(const AtomicAgent::List& agents : csg)
                {
                    ModelPool pool = AtomicAgent::getModelPool(agents);
                    if(!isSupporting(pool, resourceSet,
                                feasibilityCheckTimeoutInMs))
                    {
                        return 0.0;
                    }
                }
                return 1.0;

            });

    ModelPool::List coalitionStructure;
    std::vector<AtomicAgent::List> solution = csg.findBest(1.0);
    if(!solution.empty())
    {
        for(const AtomicAgent::List& agents : solution)
        {
            coalitionStructure.push_back( AtomicAgent::getModelPool(agents) );
        }
    }

    mpOrganizationModel->mQueryCache.cacheResult(modelPool, resourceSet, coalitionStructure);
    return coalitionStructure;
}

std::map< owlapi::model::IRI, std::map<owlapi::model::IRI, double> > OrganizationModelAsk::getPropertyValues(const
        owlapi::model::IRI& agent,
        const owlapi::model::IRI& componentKlass,
        const owlapi::model::IRI& relation) const
{

    std::map< IRI, std::map<IRI, double> > propertyValues;

    IRIList instances = mOntologyAsk.allRelatedInstances(agent, relation, componentKlass);
    for(const IRI& instance : instances)
    {
        IRISet relatedDataProperties = mOntologyAsk.getRelatedDataProperties(instance);
        for(const IRI& dataProperty : relatedDataProperties)
        {
            try {
                OWLLiteral::Ptr literal = mOntologyAsk.getDataValue(instance, dataProperty);
                double value = literal->getDouble();
                propertyValues[instance][dataProperty] = value;
            } catch(const std::exception& e)
            {
                LOG_INFO_S << "No numeric data property '" << dataProperty << "' found on instance '" << instance
                    << "' for agent model '" <<  agent << " -- " << e.what();
            }
        }
    }
    return propertyValues;
}
} // end namespace moreorg
