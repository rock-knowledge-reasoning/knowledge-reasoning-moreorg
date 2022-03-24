#ifndef ORGANIZATION_MODEL_FACADES_FACADE_HPP
#define ORGANIZATION_MODEL_FACADES_FACADE_HPP

#include <moreorg/OrganizationModelAsk.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>

namespace moreorg {
namespace facades {

class Facade
{
public:
    Facade(const OrganizationModelAsk& organizationModelAsk);

    virtual ~Facade();

    const OrganizationModelAsk& organizationAsk() const
    {
        return mOrganizationModelAsk;
    }

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
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_FACADES_FACADE_HPP
