#ifndef ORGANIZATION_MODEL_TELL_HPP
#define ORGANIZATION_MODEL_TELL_HPP

#include "OrganizationModel.hpp"

namespace moreorg {

class OrganizationModelTell
{
public:
    using Ptr = shared_ptr<OrganizationModelTell>;

    OrganizationModelTell(OrganizationModel::Ptr om);

protected:
    /**
     * Return ontology that relates to this tell object
     */
    owlapi::model::OWLOntology::Ptr ontology() const
    {
        return mpOrganizationModel->ontology();
    }

private:
    OrganizationModel::Ptr mpOrganizationModel;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_TELL_HPP
