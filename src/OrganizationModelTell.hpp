#ifndef ORGANIZATION_MODEL_TELL_HPP
#define ORGANIZATION_MODEL_TELL_HPP

#include <organization_model/OrganizationModel.hpp>

namespace organization_model {

class OrganizationModelTell
{
public:
    typedef shared_ptr<OrganizationModelTell> Ptr;

    OrganizationModelTell(OrganizationModel::Ptr om);

protected:

    /**
     * Return ontology that relates to this tell object
     */
    owlapi::model::OWLOntology::Ptr ontology() const { return mpOrganizationModel->ontology(); }

private:
    OrganizationModel::Ptr mpOrganizationModel;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_TELL_HPP
