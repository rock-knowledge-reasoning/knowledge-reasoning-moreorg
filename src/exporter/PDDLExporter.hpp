#ifndef OWL_OM_EXPORTER_PDDL_EXPORTER_HPP 
#define OWL_OM_EXPORTER_PDDL_EXPORTER_HPP 

#include <owl_om/OrganizationModel.hpp>
#include <pddl_planner/representation/Domain.hpp>

namespace owl_om {

class PDDLExporter
{

public:

    /**
     * Convert the existing model to a PDDL domain description
     * \return Domain description
     */
    pddl_planner::representation::Domain toDomain(const OrganizationModel& model);

    /**
     * Save the organization models domain description to a file
     * \param filename Name of file
     * \param model OrganizationModel that shall be saved
     */
    void save(const std::string& filename, const OrganizationModel& model);

};

} // end namespace owl_om
#endif // OWL_OM_EXPORTER_PDDL_EXPORTER_HPP 