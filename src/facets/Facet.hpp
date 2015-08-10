#ifndef ORGANIZATION_MODEL_FACETS_FACET_HPP
#define ORGANIZATION_MODEL_FACETS_FACET_HPP

#include <organization_model/OrganizationModelAsk.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>

namespace organization_model {
namespace facets {

class Facet
{
public:
    Facet(const OrganizationModel::Ptr& organizationModel)
        : mOrganizationModelAsk(organizationModel)
        , mOWLOntologyAsk(organizationModel->ontology())
    {}

protected:
    const OrganizationModelAsk& organizationAsk() const { return mOrganizationModelAsk; }
    const owlapi::model::OWLOntologyAsk& ontologyAsk() const { return mOWLOntologyAsk; }

    OrganizationModelAsk mOrganizationModelAsk;
    owlapi::model::OWLOntologyAsk mOWLOntologyAsk;


};

} // end namespace facets
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FACETS_FACET_HPP
