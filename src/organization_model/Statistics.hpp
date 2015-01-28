#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_STATISTICS_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_STATISTICS_HPP

#include <stdint.h>
#include <base/Time.hpp>
#include <organization_model/organization_model/InterfaceConnection.hpp>
#include <organization_model/organization_model/ActorModelLink.hpp>

namespace owl = owlapi::model;

namespace organization_model {
namespace organization_model {

/**
 * Statistic of the organization model engine
 */
struct Statistics
{
    Statistics();

    uint32_t upperCombinationBound;
    uint32_t numberOfInferenceEpochs;
    base::Time timeCompositeSystemGeneration;
    base::Time timeRegisterCompositeSystems;
    base::Time timeInference;
    base::Time timeElapsed;

    owl::IRIList interfaces;
    uint32_t maxAllowedLinks;
    InterfaceConnectionList links;
    InterfaceCombinationList linkCombinations;

    uint32_t constraintsChecked;

    owl::IRIList actorsAtomic;
    owl::IRIList actorsKnown;
    owl::IRIList actorsInferred;

    owl::IRIList actorsCompositePrevious;
    //owl::IRIList actorsCompositePost;
    uint32_t actorsCompositePost;

    owl::IRIList actorsCompositeModelPrevious;
    std::vector< std::vector<ActorModelLink> > actorsCompositeModelPost;
    //uint32_t actorsCompositeModelPost;

    std::string toString() const;
};

std::ostream& operator<<(std::ostream& os, const Statistics& statistics);
std::ostream& operator<<(std::ostream& os, const std::vector<Statistics>& statisticsList);

} // end namespace organization_model
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_STATISTICS_HPP
