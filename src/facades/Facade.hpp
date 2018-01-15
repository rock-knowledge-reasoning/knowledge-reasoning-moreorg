#ifndef ORGANIZATION_MODEL_FACADES_FACADE_HPP
#define ORGANIZATION_MODEL_FACADES_FACADE_HPP

#include <organization_model/OrganizationModelAsk.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>

namespace organization_model {
namespace facades {

class Facade
{
public:
    Facade(const OrganizationModelAsk& organizationModelAsk)
        : mOrganizationModelAsk(organizationModelAsk)
    {}

    const OrganizationModelAsk& organizationAsk() const { return mOrganizationModelAsk; }

protected:
    Facade()
    {}

    OrganizationModelAsk mOrganizationModelAsk;
};

} // end namespace facades
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FACADES_FACADE_HPP
