#include "PropertyBasedSelection.hpp"
#include "../facades/Robot.hpp"
#include "../vocabularies/OM.hpp"
#include "../algebra/CompositionFunction.hpp"
#include <base-logging/Logging.hpp>

using namespace owlapi::model;

namespace moreorg {
namespace policies {

PropertyBasedSelection::PropertyBasedSelection(const IRI& property,
        const IRI& op)
    : SelectionPolicy(property.toString() + "_" + op.getFragment() + "_PropertyBasedSelection")
    , mProperty(property)
    , mOperator(op)
{
}

policies::Selection PropertyBasedSelection::apply(
        const policies::Selection& selection,
        const OrganizationModelAsk& ask) const
{
    std::map<Agent, double> values;

    for(const Agent& a : selection)
    {
        facades::Robot robot = facades::Robot::getInstance(a.getType(), ask);
        // If this is a composite agent, then here, we have to decide
        // how to combine a property, e.g., for two or more system contributing
        // to the value
        // TODO: allow to configure composition function
        values[a] = robot.getCompositeDataPropertyValue(mProperty,
                    bind(&algebra::CompositionFunction::weightedSum,placeholder::_1,placeholder::_2)
                );
    }

    if(selection.empty())
    {
        return selection;
    }

    if(mOperator == vocabulary::OM::resolve("OP_MAX"))
    {
        Agent maxAgent;
        double value = 0.0;
        for(const auto& p : values)
        {
            if(p.second > value)
            {
                value = p.second;
                maxAgent = p.first;
            }
        }
        return Agent::Set({maxAgent});
    } else if(mOperator == vocabulary::OM::resolve("OP_MIN"))
    {
        Agent minAgent;
        double value = std::numeric_limits<double>::max();
        for(const auto& p : values)
        {
            if(p.second < value)
            {
                value = p.second;
                minAgent = p.first;
            }
        }

        return Agent::Set({minAgent});
    }
    throw std::runtime_error("moreorg::policies::PropertyBasedSelection::apply"
            " failed to identify operator '" + mOperator.toString() + "'");
}

} // end namespace policies
} // end namespace moreorg
