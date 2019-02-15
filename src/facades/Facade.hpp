#ifndef ORGANIZATION_MODEL_FACADES_FACADE_HPP
#define ORGANIZATION_MODEL_FACADES_FACADE_HPP

#include <organization_model/OrganizationModelAsk.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>

namespace organization_model {
namespace facades {

class Facade
{
public:
    Facade(const OrganizationModelAsk& organizationModelAsk);

    virtual ~Facade();

    const OrganizationModelAsk& organizationAsk() const { return mOrganizationModelAsk; }

    /**
     * Get the available value of use the default
     */
    double getDoubleValueOrDefault(const owlapi::model::IRI& agentModel,
            const owlapi::model::IRI& propertyName,
            double defaultValue) const;

protected:
    Facade();

    OrganizationModelAsk mOrganizationModelAsk;
};

} // end namespace facades
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FACADES_FACADE_HPP
