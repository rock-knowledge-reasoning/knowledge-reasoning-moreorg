#include "Statistics.hpp"
#include <sstream>

namespace organization_model {
namespace organization_model {

Statistics::Statistics()
    : upperCombinationBound(0)
    , numberOfInferenceEpochs(0)
    , maxAllowedLinks(0)
    , constraintsChecked(0)
    , actorsCompositePost(0)
{
}

std::string Statistics::toString() const
{
    std::stringstream txt;
    txt << "Organization Model Statistics" << std::endl
        << "    atomic actors:                      " << actorsAtomic << std::endl
        << "    upper bound for combinations:       " << upperCombinationBound << std::endl
        << "    number of inference epochs:         " << numberOfInferenceEpochs << std::endl
        << "    time elapsed in s:                  " << timeElapsed.toSeconds() << std::endl
        << "    time composite system gen in s:     " << timeCompositeSystemGeneration.toSeconds() << std::endl
        << "    time registration of comp sys:      " << timeRegisterCompositeSystems.toSeconds() << std::endl
        << "    time for inference:                 " << timeInference.toSeconds() << std::endl
        << "    # of interfaces:                    " << interfaces.size() << std::endl
        << "    # of allowed links for comps sys:   " << maxAllowedLinks << std::endl
        << "    # of (feasible) links:              " << links.size() << std::endl
        << "    # of (feasible) link combinations:  " << linkCombinations.size() << std::endl
        << "    # of constraints checked:           " << constraintsChecked << std::endl
        << "    # of known actors:                  " << actorsKnown.size() << std::endl
        << "    # of inferred actors:               " << actorsInferred.size() << std::endl
        << "    # of composite actors pre:          " << actorsCompositePrevious.size() << std::endl
        << "    # of composite actors post:         " << actorsCompositePost << std::endl
        << "    # of composite actor model pre:     " << actorsCompositeModelPrevious.size() << std::endl
        << "    # of composite actor model post:    " << actorsCompositeModelPost.size() << std::endl
        << "    composite actors:                   " << std::endl << actorsCompositeModelPost << std::endl;

    return txt.str();
}

std::ostream& operator<<(std::ostream& os, const Statistics& statistics)
{
    os << statistics.toString();
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Statistics>& statisticsList)
{
    os << "StatisticsList: " << std::endl;
    for(const Statistics& s : statisticsList)
    {
        os << s;
    }
    return os;
}

} // end namespace organization_model
} // end namespace organization_model


