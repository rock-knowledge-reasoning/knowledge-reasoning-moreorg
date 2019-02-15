#include "Facade.hpp"
#include <base-logging/Logging.hpp>

namespace organization_model {
namespace facades {

Facade::Facade(const OrganizationModelAsk& organizationModelAsk)
     : mOrganizationModelAsk(organizationModelAsk)
 {}

Facade::Facade()
{}

Facade::~Facade()
{}

double Facade::getDoubleValueOrDefault(const owlapi::model::IRI& agentModel, const
        owlapi::model::IRI& propertyName, double defaultValue) const
{
    double value;
    try {
        value = organizationAsk().ontology().getDataValue(agentModel,
                propertyName)->getDouble();
        return value;
    } catch(const std::exception& e)
    {
        LOG_INFO_S << "AgentModel '" << agentModel << "' has no property value"
            " for '" << propertyName << "'";
        return defaultValue;
    }
}


} // end namespace facades
} // end namespace organization_model

