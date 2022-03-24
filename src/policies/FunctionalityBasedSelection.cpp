#include "FunctionalityBasedSelection.hpp"
#include <base-logging/Logging.hpp>

namespace moreorg {
namespace policies {

FunctionalityBasedSelection::FunctionalityBasedSelection(
    const owlapi::model::IRI& functionality)
    : SelectionPolicy(functionality.toString() + "_FunctionalityBasedSelection")
    , mFunctionality(functionality)
{
}

policies::Selection
FunctionalityBasedSelection::apply(const policies::Selection& agents,
                                   const OrganizationModelAsk& ask) const
{
    Resource resource(mFunctionality);
    ModelPool::Set supportedModels = ask.getResourceSupport(resource);

    size_t modelCount;
    Agent::Set selectedModels;
    for(const Agent& agent : agents)
    {
        ModelPool modelPool = agent.getType();

        for(const ModelPool& supportedModel : supportedModels)
        {
            if(!Algebra::delta(supportedModel, modelPool).isNegative())
            {
                selectedModels.insert(Agent(supportedModel));
                ++modelCount;
            }
        }
    }

    return selectedModels;
}

} // end namespace policies
} // end namespace moreorg
