#ifndef ORGANIZATION_MODEL_FACETS_FACET_HPP
#define ORGANIZATION_MODEL_FACETS_FACET_HPP

#include <organization_model/OrganizationModelAsk.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>

namespace organization_model {
namespace facets {

class Facet
{
public:
    Facet(const OrganizationModelAsk& organizationModelAsk)
        : mOrganizationModelAsk(organizationModelAsk)
    {}

    const OrganizationModelAsk& organizationAsk() const { return mOrganizationModelAsk; }

protected:
    Facet()
    {}

    OrganizationModelAsk mOrganizationModelAsk;
};

} // end namespace facets
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FACETS_FACET_HPP
