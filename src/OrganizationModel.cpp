#include "OrganizationModel.hpp"
#include <sstream>
#include <owlapi/Vocabulary.hpp>

#include <owlapi/model/OWLOntologyAsk.hpp>
#include <owlapi/model/OWLOntologyTell.hpp>
#include <owlapi/model/OWLOntologyReader.hpp>
#include <owlapi/csp/ResourceMatch.hpp>
#include <base/Time.hpp>

using namespace owlapi::vocabulary;
using namespace owlapi::model;

namespace organization_model {

OrganizationModel::OrganizationModel(const std::string& filename)
    : mpOntology( new OWLOntology())
{
    if(!filename.empty())
    {
        OWLOntologyReader reader;
        mpOntology = reader.fromFile(filename);
    }

    mpTell = OWLOntologyTell::Ptr( new OWLOntologyTell(mpOntology));
    mpAsk = OWLOntologyAsk::Ptr( new OWLOntologyAsk(mpOntology));
}

void OrganizationModel::prepare(const ModelPool& modelPool)
{
    computeFunctionalityMaps(modelPool);
}

OrganizationModel OrganizationModel::copy() const
{
    OrganizationModel om;
    om.mpOntology = OWLOntology::Ptr( new OWLOntology( *ontology().get() ));
    return om;
}

owlapi::model::IRIList OrganizationModel::getServiceModels() const
{
    bool directSubclassOnly = false;
    IRIList subclasses = mpAsk->allSubclassesOf(OM::Service(), directSubclassOnly);
    return subclasses;
}

std::string OrganizationModel::toString(const Combination2FunctionMap& combinationFunctionMap)
{
    std::stringstream ss;
    ss << "combination --> functions: " << std::endl;
    Combination2FunctionMap::const_iterator cit = combinationFunctionMap.begin();
    for(; cit != combinationFunctionMap.end(); ++cit)
    {
        ss << "    combination:    " << cit->first << std::endl;
        ss << "      --> functions: " << cit->second << std::endl;
    }
    return ss.str();
}

std::string OrganizationModel::toString(const Function2CombinationMap& functionCombinationMap)
{
    std::stringstream ss;
    ss << "function --> combinations: ";
    Function2CombinationMap::const_iterator cit = functionCombinationMap.begin();
    for(; cit != functionCombinationMap.end(); ++cit)
    {
        ss << "    function:    " << cit->first << std::endl;
        ss << "        supported by:" << std::endl;

        const ModelCombinationList& combinationsList = cit->second;
        ModelCombinationList::const_iterator comIt = combinationsList.begin();
        for(; comIt != combinationsList.end(); ++comIt)
        {
            ss << "        combination:    " << *comIt << std::endl;
        }
    }

    return ss.str();
}

void OrganizationModel::computeFunctionalityMaps(const ModelPool& modelPool)
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
        IRIList supportedServiceModels = owlapi::csp::ResourceMatch::filterSupportedModels(combination, serviceModels, mpOntology);

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

} // end namespace organization_model
